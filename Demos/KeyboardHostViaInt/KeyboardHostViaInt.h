/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _KEYBOARD_HOST_VIA_INT_H_
#define _KEYBOARD_HOST_VIA_INT_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <avr/wdt.h>
		#include <stdio.h>

		#include <MyUSB/Common/ButtLoadTag.h>                     // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>             // ANSI Terminal Escape Codes
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/AT90USBXXX/Serial_Stream.h>       // Serial stream driver
		#include <MyUSB/Drivers/Board/Bicolour.h>                 // Bicolour LEDs driver
		#include <MyUSB/Scheduler/Scheduler.h>                    // Simple scheduler for task management
		
	/* Macros: */
		#define KEYBOARD_DATAPIPE              1
		#define KEYBOARD_CLASS                 0x03
		#define KEYBOARD_PROTOCOL              0x01
		
		#define MAX_CONFIG_DESCRIPTOR_SIZE     512

	/* Type Defines: */
		typedef struct
		{
			uint8_t Modifier;
			uint8_t KeyCode;
		} USB_KeyboardReport_Data_t;
		
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
		TASK(USB_Keyboard_Host);

	/* Event Handlers: */
		HANDLES_EVENT(USB_DeviceAttached);
		HANDLES_EVENT(USB_DeviceUnattached);
		HANDLES_EVENT(USB_HostError);
		HANDLES_EVENT(USB_DeviceEnumerationFailed);
		
	/* Function Prototypes: */
		uint8_t GetConfigDescriptorData(void);
		
#endif
