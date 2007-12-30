/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _TESTEVENTS_H_
#define _TESTEVENTS_H_

	/* Includes: */
		#include <avr/io.h>

		#include <MyUSB/Common/Common.h>                          // Function Attribute, Atomic, Debug and ISR Macros
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>                // Bicolour LED driver for USBKEY
		#include <MyUSB/Drivers/AT90USB_64x_128x/Serial_Stream.h> // USART Stream driver
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>             // ANSI Terminal Escape Codes

	/* Event Catch List: */
		HANDLES_EVENT(USB_VBUSChange);
		HANDLES_EVENT(USB_VBUSConnect);
		HANDLES_EVENT(USB_VBUSDisconnect);
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_Suspend);
		HANDLES_EVENT(USB_WakeUp);
		HANDLES_EVENT(USB_Reset);
		HANDLES_EVENT(USB_UIDChange);
		HANDLES_EVENT(USB_PowerOnFail);
		HANDLES_EVENT(USB_HostError);
		HANDLES_EVENT(USB_DeviceError);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		HANDLES_EVENT(USB_CreateEndpoints);
		HANDLES_EVENT(USB_DeviceAttached);
		HANDLES_EVENT(USB_DeviceUnattached);

	/* Macros: */
		#define EVENT_PREFIX ESC_INVERSE_ON "EVENT:" ESC_INVERSE_OFF " "

	/* Function Prototypes: */
		#if defined(INCLUDE_FROM_TESTEVENTS_C)
			static void Abort_Program(void) ATTR_NO_RETURN;
		#endif

#endif
