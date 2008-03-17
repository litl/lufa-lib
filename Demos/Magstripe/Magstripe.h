/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
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

		#include <MyUSB/Common/ButtLoadTag.h>         // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/Misc/Magstripe.h>     // Magstripe driver
		
		#include "../Keyboard/Keyboard.h"

	/* Function Prototypes: */
		void Keyboard_SendKeyReport(uint8_t KeyCode);		

#endif
