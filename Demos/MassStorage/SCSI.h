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

#ifndef _SCSI_H_
#define _SCSI_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>

		#include <MyUSB/Common/Common.h>              // Function Attribute, Atomic, Debug and ISR Macros
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/Board/LEDs.h>         // LEDs driver

		#include "MassStorage.h"
		#include "Descriptors.h"
		#include "DataflashManager.h"
		#include "SCSI_Codes.h"
	
	/* Macros: */
		#define SCSI_SET_SENSE(key, acode, aqual)  		   MACROS{ SenseData.SenseKey = key;              \
		                                                           SenseData.AdditionalSenseCode = acode; \
		                                                           SenseData.AdditionalSenseQualifier = aqual; }MACROE

		#define DATA_READ                                  true
		#define DATA_WRITE                                 false
		
	/* Type Defines: */
		typedef struct
		{
			unsigned int DeviceType          : 5;
			unsigned int PeripheralQualifier : 3;
			
			unsigned int _RESERVED1          : 7;
			unsigned int Removable           : 1;
			
			uint8_t      Version;
			
			unsigned int ResponseDataFormat  : 4;
			unsigned int _RESERVED2          : 1;
			unsigned int NormACA             : 1;
			unsigned int TrmTsk              : 1;
			unsigned int AERC                : 1;

			uint8_t      AdditionalLength;
			uint8_t      _RESERVED3[2];

			unsigned int SoftReset           : 1;
			unsigned int CmdQue              : 1;
			unsigned int _RESERVED4          : 1;
			unsigned int Linked              : 1;
			unsigned int Sync                : 1;
			unsigned int WideBus16Bit        : 1;
			unsigned int WideBus32Bit        : 1;
			unsigned int RelAddr             : 1;
			
			uint8_t      VendorID[8];
			uint8_t      ProductID[16];
			uint8_t      RevisionID[4];
		} SCSI_Inquiry_Response_t;
		
		typedef struct
		{
			uint8_t      ResponseCode;
			
			uint8_t      SegmentNumber;
			
			unsigned int SenseKey            : 4;
			unsigned int _RESERVED1          : 1;
			unsigned int ILI                 : 1;
			unsigned int EOM                 : 1;
			unsigned int FileMark            : 1;
			
			uint8_t      Information[4];
			uint8_t      AdditionalLength;
			uint8_t      CmdSpecificInformation[4];
			uint8_t      AdditionalSenseCode;
			uint8_t      AdditionalSenseQualifier;
			uint8_t      FieldReplaceableUnitCode;
			uint8_t      SenseKeySpecific[3];
		} SCSI_Request_Sense_Response_t;
		
	/* Function Prototypes: */
		void SCSI_DecodeSCSICommand(void);
		
		#if defined(INCLUDE_FROM_SCSI_C)
			static bool SCSI_Command_Inquiry(void);
			static bool SCSI_Command_Request_Sense(void);
			static bool SCSI_Command_Read_Capacity_10(void);
			static bool SCSI_Command_Send_Diagnostic(void);
			static bool SCSI_Command_ReadWrite_10(const bool IsDataRead);
		#endif
		
#endif
