/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __HOSTCHAP9_H__
#define __HOSTCHAP9_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		
		#include "LowLevel.h"
		#include "StdRequestType.h"

	/* Public Interface - May be used in end-application: */
		/* Type Defines: */
			typedef struct
			{
				uint8_t  RequestType;
				uint8_t  RequestData;
				uint16_t Value;
				uint16_t Index;
				uint16_t DataLength;
			} USB_Host_Request_Header_t;

		/* Enums: */
			enum USB_Host_SendControlErrorCodes_t
			{
				HOST_SENDCONTROL_Sucessful        = 0,
				HOST_SENDCONTROL_DeviceDisconnect = 1,
				HOST_SENDCONTROL_PipeError        = 2,
				HOST_SENDCONTROL_SetupStalled     = 3,
				HOST_SENDCONTROL_SoftwareTimeOut  = 4,
			};
			
		/* Global Variables: */
			extern USB_Host_Request_Header_t USB_HostRequest;
			
		/* Function Prototypes: */
			uint8_t USB_Host_SendControlRequest(void* BufferPtr);
			
	/* Private Interface - For use in library only: */
		/* Enums: */
			enum WaitForTypes_t
			{
				Wait_For_Setup_Sent,
				Wait_For_In_Received,
				Wait_For_Out_Ready,
			};
	
		/* Function Prototypes: */
			#if defined(INCLUDE_FROM_HOSTCHAPTER9_C)
				static uint8_t USB_Host_Wait_For_Setup_IOS(uint8_t WaitType);
			#endif

#endif
