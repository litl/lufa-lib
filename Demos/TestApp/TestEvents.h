/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _TESTEVENTS_H_
#define _TESTEVENTS_H_

	/* Includes: */
		#include <avr/io.h>

		#include <MyUSB/Common/Common.h>                          // Commonly used macros
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/Board/LEDs.h>                     // LED driver
		#include <MyUSB/Drivers/AT90USBXXX/Serial_Stream.h>       // USART Stream driver
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>             // ANSI Terminal Escape Codes
		#include <MyUSB/Scheduler/Scheduler.h>                    // Simple scheduler for task management

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
		HANDLES_EVENT(USB_DeviceEnumerationFailed);
		HANDLES_EVENT(USB_DeviceError);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		HANDLES_EVENT(USB_ConfigurationChanged);
		HANDLES_EVENT(USB_DeviceAttached);
		HANDLES_EVENT(USB_DeviceUnattached);

	/* Macros: */
		#define EVENT_PREFIX ESC_INVERSE_ON "EVENT:" ESC_INVERSE_OFF " "

	/* Function Prototypes: */
		#if defined(INCLUDE_FROM_TESTEVENTS_C)
			static void Abort_Program(void) ATTR_NO_RETURN;
		#endif

#endif
