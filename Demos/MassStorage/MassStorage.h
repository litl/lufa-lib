/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
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
		enum CommandStatusCodes_t
		{
			Command_Pass = 0,
			Command_Fail = 1,
			Phase_Error  = 2
		};
		
	/* Global Variables: */
		extern CommandBlockWrapper_t  CommandBlock;
		extern CommandStatusWrapper_t CommandStatus;

	/* Task Definitions: */
		TASK(USB_MassStorage);

	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_ConfigurationChanged);
		HANDLES_EVENT(USB_UnhandledControlPacket);

	/* Function Prototypes: */
		#if defined(INCLUDE_FROM_MASSSTORAGE_C)
			static bool ReadInCommandBlock(void);
			static void ReturnCommandStatus(void);
		#endif

#endif
