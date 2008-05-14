/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
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
		
		#define MODE_10                                    true
		#define MODE_6                                     false
		
		#define INFORMATIONAL_PAGE_OVERHEAD(pages)         (3 + (2 * pages))
		
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
			unsigned int ReponseCode         : 7;
			unsigned int Valid               : 1;
			
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
		
		typedef struct
		{
			unsigned int DCR  : 1;
			unsigned int DTE  : 1;
			unsigned int PER  : 1;
			unsigned int EER  : 1;
			unsigned int RC   : 1;
			unsigned int TB   : 1;
			unsigned int ARRE : 1;
			unsigned int AWRE : 1;

			uint8_t      ReadRetryCount;
			
			uint8_t      _RESERVED1[4];
			
			uint8_t      WriteRetryCount;
			
			uint8_t      _RESERVED2;

			uint16_t     RecoveryTimeLimit;
		} SCSI_Read_Write_Error_Recovery_Sense_Page_t;
		
		typedef struct
		{
			unsigned int LogErr      : 1;
			unsigned int _RESERVED1  : 1;
			unsigned int Test        : 1;
			unsigned int Dexcpt      : 1;
			unsigned int _RESERVED2  : 3;
			unsigned int Perf        : 1;
			
			unsigned int MRIE        : 4;
			unsigned int _RESERVED3  : 4;

			uint32_t     IntervalTimer;
		
			uint32_t     ReportCount;
		} SCSI_Informational_Exceptions_Sense_Page_t;
		
	/* Function Prototypes: */
		void SCSI_DecodeSCSICommand(void);
		
		#if defined(INCLUDE_FROM_SCSI_C)
			static bool SCSI_Command_Inquiry(void);
			static bool SCSI_Command_Request_Sense(void);
			static bool SCSI_Command_Read_Capacity_10(void);
			static bool SCSI_Command_Send_Diagnostic(void);
			static bool SCSI_Command_ReadWrite_6_10(const bool IsDataRead, const bool IsMode10);
			static bool SCSI_Command_Mode_Sense_6_10(const bool IsMode10);
			static void SCSI_WriteSensePageHeader(const uint8_t DataLength, const bool IsMode10);
			static void SCSI_WriteSensePage(const uint8_t PageCode, const uint8_t PageSize,
											void* PageDataPtr,
			                                const int16_t AllocationLength,
											const bool IsMode10)
			                                ATTR_NON_NULL_PTR_ARG(3);
		#endif
		
#endif
