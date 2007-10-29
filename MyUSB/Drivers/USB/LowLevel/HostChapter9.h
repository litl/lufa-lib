/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef HOSTCHAP9_H
#define HOSTCHAP9_H

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		
		#include "LowLevel.h"
		#include "StdRequestType.h"

	/* Private Interface - For use in library only: */
		/* Type Defines */
			typedef struct
			{
				uint8_t  bmRequestType;
				uint8_t  bRequest;
				uint16_t wValue;
				uint16_t wIndex;
				uint16_t wLength;
			} USB_Host_Request_Header_t;
		
		/* Global Variables */
			extern USB_Host_Request_Header_t USB_HostRequest;
			
		/* Function Prototypes */
			void USB_Host_SendControlRequest(const void* Data);

#endif
