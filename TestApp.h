/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef TESTAPP_H
#define TESTAPP_H

	/* Includes */
		#include <avr/io.h>
		#include <avr/interrupt.h>

		#include "MyUSB/Scheduler/Scheduler.h"            // Simple scheduler for task management
		#include "MyUSB/Common/ISRMacro.h"                // Replacement ISR macro

		#include "MyUSB/Drivers/USB/USB.h"                // USB Functionality

		#include "MyUSB/Drivers/USB1287/ADC.h"            // ADC driver
		#include "MyUSB/Drivers/USB1287/Serial_Stream.h"  // USART Stream driver

		#include "MyUSB/Drivers/USBKEY/Joystick.h"        // Joystick driver for the USBKEY
		#include "MyUSB/Drivers/USBKEY/Bicolour.h"        // Bicolour LEDs driver for the USBKEY
		#include "MyUSB/Drivers/USBKEY/HWB.h"             // Hardware Button driver for the USBKEY
		#include "MyUSB/Drivers/USBKEY/Temperature.h"     // Temperature sensor driver
		
	/* Task Definitions */
		TASK(TestApp_CheckJoystick);
		TASK(TestApp_CheckHWB);

	/* Scheduler Task List: */
		TASK_ID_LIST
		{
			TestApp_CheckJoystick_ID,
			TestApp_CheckHWB_ID,
			USB_USBTask_ID,
		};

		TASK_LIST
		{
			{ TaskID: TestApp_CheckJoystick_ID, TaskName: TestApp_CheckJoystick, TaskStatus: TASK_RUN  },
			{ TaskID: TestApp_CheckHWB_ID     , TaskName: TestApp_CheckHWB     , TaskStatus: TASK_RUN  },
			{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
		};

#endif
