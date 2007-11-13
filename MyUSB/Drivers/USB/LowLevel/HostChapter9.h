/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __HOSTCHAP9_H__
#define __HOSTCHAP9_H__

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		
		#include "LowLevel.h"
		#include "StdRequestType.h"

	/* Public Interface - May be used in end-application: */
		/* Type Defines: */
			typedef struct
			{
				uint8_t  RequestType;
				uint8_t  RequestData;
				
				union
				{
					uint16_t Value;
					
					struct
					{
						uint8_t Value_HighByte;
						uint8_t Value_LowByte;
					};
				};

				uint16_t Index;
				uint16_t Length;
			} USB_Host_Request_Header_t;
		
		/* Global Variables: */
			extern USB_Host_Request_Header_t USB_HostRequest;
			
		/* Function Prototypes: */
			void USB_Host_SendControlRequest(const void* Data);

#endif
