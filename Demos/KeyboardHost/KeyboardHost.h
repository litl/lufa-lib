/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef _MOUSE_H_
#define _MOUSE_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#include <stdio.h>

		#include <MyUSB/Common/ButtLoadTag.h>             // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>     // ANSI Terminal Escape Codes
		#include <MyUSB/Drivers/USB/USB.h>                // USB Functionality
		#include <MyUSB/Drivers/USB1287/Serial_Stream.h>  // Serial stream driver for the USB1287
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>        // Bicolour LEDs driver for the USBKEY
		#include <MyUSB/Scheduler/Scheduler.h>            // Simple scheduler for task management
		
	/* Macros */
		#define KEYBOARD_DATAPIPE              1
		#define KEYBOARD_CLASS                 0x03
		#define KEYBOARD_PROTOCOL              0x01
		
	/* Type Defines: */
		typedef struct
		{
			uint8_t Modifier;
			uint8_t KeyCode;
		} USB_KeyboardReport_Data_t;
		
	/* Task Definitions: */
		TASK(USB_Keyboard_Host);

	/* Event Handlers: */
		HANDLES_EVENT(USB_DeviceAttached);
		HANDLES_EVENT(USB_DeviceUnattached);
		HANDLES_EVENT(USB_HostError);
		
#endif
