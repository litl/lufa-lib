#ifndef TESTAPP_H
#define TESTAPP_H

	/* Includes */
		#include <avr/io.h>
		#include <avr/interrupt.h>

		#include "MyUSB/Common/Scheduler.h"           // Simple scheduler for task management
		#include "MyUSB/Common/ISRMacro.h"            // Replacement ISR macro

		#include "MyUSB/Drivers/USB/USB.h"            // USB Functionality

		#include "MyUSB/Drivers/USB1287/ADC.h"        // A/D Routines for the USB1287

		#include "MyUSB/Drivers/USBKEY/Joystick.h"    // Joystick driver for the USBKEY
		#include "MyUSB/Drivers/USBKEY/Bicolour.h"    // Bicolour LEDs driver for the USBKEY
		#include "MyUSB/Drivers/USBKEY/HWB.h"         // Hardware Button driver for the USBKEY
		#include "MyUSB/Drivers/USBKEY/Temperature.h" // Temperature Sensor driver for the USBKEY

	/* Task Definitions */
		TASK(TestApp_CheckJoystick);
		TASK(TestApp_CheckHWB);

#endif
