/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef DEVCHAP9_H
#define DEVCHAP9_H

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		
		#include "../../../Configuration/USB/Device/Descriptors.h"
		#include "../HighLevel/Events.h"
		#include "LowLevel.h"
		#include "StdRequestType.h"

	/* Public Interface - May be used in end-application: */
		/* Global Variables */
			extern uint8_t USB_ConfigurationNumber;

		/* Throwable Events */
			RAISES_EVENT(USB_UnhandledControlPacket);
			RAISES_EVENT(USB_CreateEndpoints);

	/* Private Interface - For use in library only: */	
		/* Function Prototypes */
			void USB_Device_ProcessControlPacket(void);
			void USB_Device_SetAddress(void);
			void USB_Device_SetConfiguration(void);
			void USB_Device_GetConfiguration(void);
			void USB_Device_GetDescriptor(void);
			void USB_Device_GetStatus(const uint8_t RequestType);
			void USB_Device_SetFeature(const uint8_t RequestType);
			void USB_Device_ClearFeature(const uint8_t RequestType);
		
#endif
