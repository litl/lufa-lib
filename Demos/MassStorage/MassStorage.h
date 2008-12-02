/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _MASS_STORAGE_H_
#define _MASS_STORAGE_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>

		#include "Descriptors.h"
		#include "SCSI.h"
		#include "DataflashManager.h"

		#include <MyUSB/Version.h>                    // Library Version Information
		#include <MyUSB/Common/ButtLoadTag.h>         // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/Board/LEDs.h>         // LEDs driver
		#include <MyUSB/Drivers/Board/Dataflash.h>    // Dataflash chip driver
		#include <MyUSB/Scheduler/Scheduler.h>        // Simple scheduler for task management

	/* Macros: */
		#define MASS_STORAGE_RESET         0xFF
		#define GET_MAX_LUN                0xFE

		#define MAX_SCSI_COMMAND_LENGTH    16
		
		#define TOTAL_LUNS                 2
		#define LUN_MEDIA_SIZE             (VIRTUAL_MEMORY_BLOCKS / TOTAL_LUNS)    
		
		#define CBW_SIGNATURE              0x43425355UL
		#define CSW_SIGNATURE              0x53425355UL
		
		#define COMMAND_DIRECTION_DATA_OUT (0 << 7)
		#define COMMAND_DIRECTION_DATA_IN  (1 << 7)

	/* Type defines: */
		typedef struct
		{
			struct
			{
				uint32_t Signature;
				uint32_t Tag;
				uint32_t DataTransferLength;
				uint8_t  Flags;
				uint8_t  LUN;
				uint8_t  SCSICommandLength;			
			} Header;
			
			uint8_t SCSICommandData[16];
		} CommandBlockWrapper_t;
		
		typedef struct
		{
			struct
			{
				const uint32_t Signature;
					  uint32_t Tag;
					  uint32_t SCSICommandResidue;
					  uint8_t  Status;
			} Header;
		} CommandStatusWrapper_t;
		
	/* Enums: */
		enum MassStorage_CommandStatusCodes_t
		{
			Command_Pass = 0,
			Command_Fail = 1,
			Phase_Error  = 2
		};

		/** Enum for the possible status codes for passing to the UpdateStatus() function. */
		enum StatusCodes_t
		{
			Status_USBNotReady            = 0, /**< USB is not ready (disconnected from a USB host) */
			Status_USBEnumerating         = 1, /**< USB interface is enumerating */
			Status_USBReady               = 2, /**< USB interface is connected and ready */
			Status_CommandBlockError      = 3, /**< Processing a SCSI command block from the host */
			Status_ProcessingCommandBlock = 4, /**< Error during the processing of a SCSI command block from the host */
		};
		
	/* Global Variables: */
		extern CommandBlockWrapper_t  CommandBlock;
		extern CommandStatusWrapper_t CommandStatus;
		extern volatile bool          IsMassStoreReset;

	/* Task Definitions: */
		TASK(USB_MassStorage);
		
	/* Stream Callbacks: */
		STREAM_CALLBACK(AbortOnMassStoreReset);

	/* Event Handlers: */
		HANDLES_EVENT(USB_Reset);
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_ConfigurationChanged);
		HANDLES_EVENT(USB_UnhandledControlPacket);

	/* Function Prototypes: */
		/** Function prototype for the UpdateStatus() routine, to display status changes to the user. */
		void UpdateStatus(uint8_t CurrentStatus);

		#if defined(INCLUDE_FROM_MASSSTORAGEDUALLUN_C)
			static bool ReadInCommandBlock(void);
			static void ReturnCommandStatus(void);
		#endif

#endif
