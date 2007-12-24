/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __HOSTCHAP9_H__
#define __HOSTCHAP9_H__

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
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
			enum USB_Host_SendControlErrorCodes
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
			uint8_t USB_Host_SendControlRequest(uint8_t* DataBuffer);

#endif
