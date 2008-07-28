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

#ifndef _TESTAPP_H_
#define _TESTAPP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <avr/wdt.h>

		#include <MyUSB/Version.h>                               // Library Version Information
		#include <MyUSB/Drivers/USB/USB.h>                       // USB Functionality
		#include <MyUSB/Scheduler/Scheduler.h>                   // Simple scheduler for task management
		#include <MyUSB/MemoryAllocator/DynAlloc.h>              // Auto-defragmenting Dynamic Memory allocation
		#include <MyUSB/Common/ButtLoadTag.h>                    // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>            // ANSI Terminal Escape Codes
		#include <MyUSB/Drivers/AT90USBXXX/ADC.h>                // ADC driver
		#include <MyUSB/Drivers/AT90USBXXX/Serial_Stream.h>      // USART Stream driver
		#include <MyUSB/Drivers/Board/Joystick.h>                // Joystick driver
		#include <MyUSB/Drivers/Board/LEDs.h>                    // LED driver
		#include <MyUSB/Drivers/Board/HWB.h>                     // Hardware Button driver
		#include <MyUSB/Drivers/Board/Temperature.h>             // Temperature sensor driver
		
	/* Task Definitions: */
		TASK(TestApp_CheckJoystick);
		TASK(TestApp_CheckHWB);
		TASK(TestApp_CheckTemp);

#endif
