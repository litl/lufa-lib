/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _MOUSE_HOST_H_
#define _MOUSE_HOST_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <stdio.h>

		#include <MyUSB/Common/ButtLoadTag.h>                     // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>             // ANSI Terminal Escape Codes
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/AT90USB_64x_128x/Serial_Stream.h> // Serial stream driver for the USB1287
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>                // Bicolour LEDs driver for the USBKEY
		#include <MyUSB/Scheduler/Scheduler.h>                    // Simple scheduler for task management
		
	/* Macros: */
		#define MOUSE_DATAPIPE              1
		#define MOUSE_CLASS                 0x03
		#define MOUSE_PROTOCOL              0x02

		#define MAX_CONFIG_DESCRIPTOR_SIZE  512

	/* Type Defines: */
		typedef struct
		{
			uint8_t Button;
			int8_t  X;
			int8_t  Y;
		} USB_MouseReport_Data_t;
		
	/* Enums: */
		enum
		{
			ControlError         = 0,
			DescriptorTooLarge   = 1,
			HIDInterfaceNotFound = 2,
			IncorrectProtocol    = 3,
			NoEndpointFound      = 4,
			SuccessfulConfigRead = 5,
		} GetConfigDescriptorDataCodes_t;

	/* Task Definitions: */
		TASK(USB_Mouse_Host);

	/* Event Handlers: */
		HANDLES_EVENT(USB_DeviceAttached);
		HANDLES_EVENT(USB_DeviceUnattached);
		HANDLES_EVENT(USB_HostError);
		HANDLES_EVENT(USB_DeviceEnumerationFailed);
		
	/* Function Prototypes: */
		uint8_t GetConfigDescriptorData(void);
		
#endif
