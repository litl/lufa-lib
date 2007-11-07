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
			extern volatile uint8_t USB_HostState;

		/* Throwable Events: */
			RAISES_EVENT(USB_Connect);
			RAISES_EVENT(USB_HostError);
			RAISES_EVENT(USB_DeviceAttached);
			RAISES_EVENT(USB_DeviceUnattached);

		/* Tasks: */
			TASK(USB_USBTask);

	/* Private Interface - For use in library only: */
		/* Function Prototypes: */
			void USB_InitTaskPointer(void);
			void USB_DeviceTask(void);
			void USB_HostTask(void);

#endif
