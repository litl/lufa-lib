/*
  Copyright 2008  Denver Gingerich (denver [at] ossguy [dot] com)

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

/*
	Demonstration application for a TTL magnetic stripe reader (such as the
	Omron V3B-4K) by Denver Gingerich. See http://ossguy.com/ss_usb/ for the
	demonstration project website, including construction and support details.

	This example is based on the MyUSB Keyboard demonstration application,
	written by Denver Gingerich.
*/

#ifndef _MAGSTRIPE_H_
#define _MAGSTRIPE_H_

	/* Macros: */
		#define MAG_DATA_PIN	PORTC0
		#define MAG_CLOCK_PIN	PORTC3
		#define MAG_CLS_PIN	    PORTC4
		#define MAG_PIN		    PINC
		#define MAG_DDR		    DDRC
		#define MAG_PORT	    PORTC

		#define DATA_LENGTH     1024

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <string.h>

		#include "MagstripeHW.h"		
		#include "Descriptors.h"

		#include <MyUSB/Version.h>                    // Library Version Information
		#include <MyUSB/Common/ButtLoadTag.h>         // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/Board/Joystick.h>     // Joystick driver
		#include <MyUSB/Drivers/Board/LEDs.h>         // LEDs driver
		#include <MyUSB/Scheduler/Scheduler.h>        // Simple scheduler for task management
		
	/* Function Prototypes: */
		void Keyboard_SendKeyReport(uint8_t KeyCode);		

	/* Task Definitions: */
		TASK(USB_Keyboard_Report);

	/* Macros: */
		#define REQ_GetReport      0x01
		#define REQ_SetReport      0x09
		#define REQ_GetProtocol    0x03
		#define REQ_SetProtocol    0x0B

	/* Type Defines: */
		typedef struct
		{
			uint8_t Modifier;
			uint8_t Reserved;
			uint8_t KeyCode;
		} USB_KeyboardReport_Data_t;
			
	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		HANDLES_EVENT(USB_ConfigurationChanged);

#endif
