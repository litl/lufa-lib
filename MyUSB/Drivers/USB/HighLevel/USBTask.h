/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef USBTASK_H
#define USBTASK_H

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		
		#include "../HighLevel/Events.h"
		#include "../../../Scheduler/Scheduler.h"
		#include "../LowLevel/LowLevel.h"
		
	/* Public Interface - May be used in end-application: */
		/* Global Variables: */
			extern volatile bool    USB_IsConnected;
			extern volatile bool    USB_IsInitialized;

			#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
			extern volatile uint8_t USB_HostState;
			#endif

		/* Throwable Events: */
			RAISES_EVENT(USB_Connect);

			#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
				RAISES_EVENT(USB_HostError);
				RAISES_EVENT(USB_DeviceAttached);
				RAISES_EVENT(USB_DeviceUnattached);
			#endif

		/* Tasks: */
			TASK(USB_USBTask);

	/* Private Interface - For use in library only: */
		/* Function Prototypes: */
			void USB_DeviceTask(void);
			void USB_HostTask(void);

			#if !defined(USB_DEVICE_ONLY) && !defined(USB_HOST_ONLY) // All modes
			void USB_InitTaskPointer(void);
			#endif

#endif
