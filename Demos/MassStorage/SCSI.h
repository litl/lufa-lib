/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef _SCSI_H_
#define _SCSI_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>

		#include "MassStorage.h"
		#include "Descriptors.h"
	
	/* Macros: */
		#define SCSI_CMD_INQUIRY                 0x12
		
	/* Type Defines: */
		typedef struct
		{
			unsigned int DeviceType          : 5;
			unsigned int PeripheralQualifier : 3;
			
			unsigned int _RESERVED1          : 6;
			unsigned int Removable           : 1;
			
			unsigned int ANSI_Version        : 3;
			unsigned int ECMA_Version        : 3;
			unsigned int ISO_IEC_Version     : 2;
			
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
	
	/* Function Prototypes: */
		void DecodeSCSICommand(void);

#endif
