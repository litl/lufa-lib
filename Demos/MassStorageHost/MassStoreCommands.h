/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _MASS_STORE_COMMANDS_H_
#define _MASS_STORE_COMMANDS_H_

	/* Includes: */
		#include <avr/io.h>

		#include "MassStorageHost.h"
		#include "../MassStorage/SCSI_Codes.h"

		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality

	/* Macros: */
		#define MASS_STORAGE_RESET               0xFF
		#define GET_MAX_LUN                      0xFE

		#define CBW_SIGNATURE                    0x43425355UL // USBC
		#define CSW_SIGNATURE                    0x53425355UL // USBS
		
		#define COMMAND_DIRECTION_DATA_OUT       (0 << 7)
		#define COMMAND_DIRECTION_DATA_IN        (1 << 7)
		
		#define COMMAND_DATA_TIMEOUT_MS          5000

		#define MASS_STORE_DATA_IN_PIPE          0x01
		#define MASS_STORE_DATA_OUT_PIPE         0x02
		
		#define DEVICE_BLOCK_SIZE                512

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
			uint32_t Signature;
			uint32_t Tag;
			uint32_t SCSICommandResidue;
			uint8_t  Status;			
		} CommandStatusWrapper_t;
		
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
			uint32_t Blocks;
			uint32_t BlockSize;
		} SCSI_Capacity_t;

	/* Enums: */
		enum
		{
			Command_Pass = 0,
			Command_Fail = 1,
			Phase_Error  = 2
		} CommandStatusCodes_t;
		
		enum
		{
			NoError            = 0,
			InPipeStalled      = 1,
			OutPipeStalled     = 2,
			DeviceDisconnected = 3,
			CommandTimeout     = 4
		} ReadWriteErrorCodes_t;
		
	/* External Variables: */
		extern CommandStatusWrapper_t SCSICommandStatus;
		
	/* Function Prototypes: */
		void    MassStore_SendCommand(void);
		uint8_t MassStore_WaitForDataRecieved(void);
		uint8_t MassStore_SendRecieveData(uint8_t* BufferPtr) ATTR_NON_NULL_PTR_ARG(1);
		void    MassStore_GetReturnedStatus(void);
		
		uint8_t MassStore_ClearPipeStall(const uint8_t PipeEndpointNum);

		uint8_t MassStore_RequestSense(const uint8_t LUNIndex, SCSI_Request_Sense_Response_t* SensePtr)
		                               ATTR_NON_NULL_PTR_ARG(2);
		uint8_t MassStore_ReadDeviceBlock(const uint8_t LUNIndex, const uint32_t BlockAddress,
		                                  const uint8_t Blocks, uint8_t* BufferPtr) ATTR_NON_NULL_PTR_ARG(4);
		uint8_t MassStore_WriteDeviceBlock(const uint8_t LUNIndex, const uint32_t BlockAddress,
                                           const uint8_t Blocks, uint8_t* BufferPtr);
		uint8_t MassStore_ReadCapacity(const uint8_t LUNIndex, SCSI_Capacity_t* CapacityPtr)
		                               ATTR_NON_NULL_PTR_ARG(2);
		void    MassStore_TestUnitReady(const uint8_t LUNIndex);
		void    MassStore_PreventAllowMediumRemoval(const uint8_t LUNIndex, const bool PreventRemoval);

#endif
