/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Keyboard demonstration application by Denver Gingerich.

	This example is based on the MyUSB Mouse demonstration application,
	written by Dean Camera.
*/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>

		#include "Descriptors.h"

		#include <MyUSB/Common/ButtLoadTag.h>         // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/USBKEY/Joystick.h>    // Joystick driver for the USBKEY
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>    // Bicolour LEDs driver for the USBKEY
		#include <MyUSB/Scheduler/Scheduler.h>        // Simple scheduler for task management
		
	/* Task Definitions: */
		TASK(USB_Keyboard_Report);

	/* Type Defines: */
		typedef struct
		{
			uint8_t Modifier;
			uint8_t KeyCode;
		} USB_KeyboardReport_Data_t;
			
	/* Event Handlers: */
		HANDLES_EVENT(USB_CreateEndpoints);

#endif
