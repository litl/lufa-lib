/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _STILL_IMAGE_COMMANDS_H_
#define _STILL_IMAGE_COMMANDS_H_

	/* Includes: */
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		
		#include "PIMACodes.h"

	/* Macros: */
		#define SIMAGE_DATA_IN_PIPE            0x01
		#define SIMAGE_DATA_OUT_PIPE           0x02
		#define SIMAGE_EVENTS_PIPE             0x03

		#define COMMAND_DATA_TIMEOUT_MS        5000
		
		#define PIMA_COMMAND_SIZE(params)      ((sizeof(PIMA_SendBlock) - sizeof(PIMA_SendBlock.Params)) + \
		                                        (params * sizeof(PIMA_SendBlock.Params[0])))

	/* Type Defines: */
		typedef struct
		{
			uint32_t DataLength;
			uint16_t Type;
			uint16_t Code;
			uint32_t TransactionID;
			uint32_t Params[4];
		} PIMA_Container_t;
	
	/* Enums: */
		enum PIMA_Container_Types_t
		{
			CType_Undefined         = 0,
			CType_CommandBlock      = 1,
			CType_DataBlock         = 2,
			CType_ResponseBlock     = 3,
			CType_EventBlock        = 4,
		};

		enum ReadWriteErrorCodes_t
		{
			NoError            = 0,
			InPipeStalled      = 1,
			OutPipeStalled     = 2,
			DeviceDisconnected = 3,
			CommandTimeout     = 4,
			BadResponse        = 5,
		};
	
	/* External Variables: */
		extern PIMA_Container_t PIMA_SendBlock;
		extern PIMA_Container_t PIMA_RecievedBlock;
		extern PIMA_Container_t PIMA_EventBlock;
	
	/* Function Prototypes: */
		void    SImage_SendBlockHeader(void);
		uint8_t SImage_RecieveBlockHeader(void);
		void    SImage_RecieveEventHeader(void);
		void    SImage_SendData(void* Buffer, uint16_t Bytes);
		uint8_t SImage_ReadData(void* Buffer, uint16_t Bytes);
		bool    SImage_IsEventReceived(void);
		uint8_t SImage_ClearPipeStall(const uint8_t PipeEndpointNum);

#endif
