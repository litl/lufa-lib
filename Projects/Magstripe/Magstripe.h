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
		#define REQ_GetReport   0x01

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
