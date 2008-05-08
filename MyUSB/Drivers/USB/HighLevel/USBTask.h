/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __USBTASK_H__
#define __USBTASK_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		#include <stddef.h>
		
		#include "../../../Scheduler/Scheduler.h"
		#include "../LowLevel/LowLevel.h"
		#include "../LowLevel/HostChapter9.h"
		#include "../LowLevel/USBMode.h"
		#include "Events.h"
		#include "StdDescriptors.h"
		
	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Public Interface - May be used in end-application: */
		/* Global Variables: */
			extern volatile bool    USB_IsConnected;
			extern volatile bool    USB_IsInitialized;

			#if defined(USB_CAN_BE_HOST)
			extern volatile uint8_t USB_HostState;
			#endif

		/* Throwable Events: */
			RAISES_EVENT(USB_Connect);

			#if defined(USB_CAN_BE_HOST)
				RAISES_EVENT(USB_HostError);
				RAISES_EVENT(USB_DeviceAttached);
				RAISES_EVENT(USB_DeviceUnattached);
				RAISES_EVENT(USB_DeviceEnumerationFailed);
				RAISES_EVENT(USB_DeviceEnumerationComplete);
				RAISES_EVENT(USB_Disconnect);
			#endif

		/* Tasks: */
			TASK(USB_USBTask);

	/* Private Interface - For use in library only: */
		/* Function Prototypes: */
			#if defined(INCLUDE_FROM_USBTASK_C)
				#if defined(USB_CAN_BE_HOST)
					static void USB_HostTask(void);
				#endif
				
				#if defined(USB_CAN_BE_DEVICE)
					static void USB_DeviceTask(void);
				#endif
			#endif

			#if defined(USB_CAN_BE_BOTH)
				void USB_InitTaskPointer(void);
			#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
		
#endif
