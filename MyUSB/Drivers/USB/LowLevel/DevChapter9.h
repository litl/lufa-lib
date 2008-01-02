/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __DEVCHAP9_H__
#define __DEVCHAP9_H__

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		
		#include "../HighLevel/StdDescriptors.h"
		#include "../HighLevel/Events.h"
		#include "LowLevel.h"
		#include "StdRequestType.h"

	/* Public Interface - May be used in end-application: */
		/* Global Variables: */
			extern uint8_t USB_ConfigurationNumber;

		/* Throwable Events: */
			RAISES_EVENT(USB_UnhandledControlPacket);
			RAISES_EVENT(USB_CreateEndpoints);
	
	/* Private Interface - For use in library only: */
		/* Function Prototypes: */
			void USB_Device_ProcessControlPacket(void);
			
			#if defined(INCLUDE_FROM_DECCHAPTER9_C)
				static void USB_Device_SetAddress(void);
				static void USB_Device_SetConfiguration(void);
				static void USB_Device_GetConfiguration(void);
				static void USB_Device_GetDescriptor(void);
				static void USB_Device_GetStatus(const uint8_t RequestType);
				static void USB_Device_SetFeature(const uint8_t RequestType);
				static void USB_Device_ClearFeature(const uint8_t RequestType);
			#endif
		
#endif
