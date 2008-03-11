/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _TESTAPP_H_
#define _TESTAPP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <avr/wdt.h>

		#include <MyUSB/Drivers/USB/USB.h>                       // USB Functionality
		#include <MyUSB/Scheduler/Scheduler.h>                   // Simple scheduler for task management
		#include <MyUSB/MemoryAllocator/DynAlloc.h>              // Auto-defragmenting Dynamic Memory allocation
		#include <MyUSB/Common/ButtLoadTag.h>                    // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>            // ANSI Terminal Escape Codes
		#include <MyUSB/Drivers/AT90USBXXx/ADC.h>                // ADC driver
		#include <MyUSB/Drivers/AT90USBXXx/Serial_Stream.h>      // USART Stream driver
		#include <MyUSB/Drivers/Board/Joystick.h>                // Joystick driver
		#include <MyUSB/Drivers/Board/Bicolour.h>                // Bicolour LEDs driver
		#include <MyUSB/Drivers/Board/HWB.h>                     // Hardware Button driver
		#include <MyUSB/Drivers/Board/Temperature.h>             // Temperature sensor driver
		
	/* Task Definitions: */
		TASK(TestApp_CheckJoystick);
		TASK(TestApp_CheckHWB);
		TASK(TestApp_CheckTemp);

#endif
