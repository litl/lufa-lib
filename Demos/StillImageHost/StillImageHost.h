/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _STILL_IMAGE_HOST_H_
#define _STILL_IMAGE_HOST_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <stdio.h>
		
		#include "PIMACodes.h"
		#include "StillImageCommands.h"

		#include <MyUSB/Common/ButtLoadTag.h>                     // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>             // ANSI Terminal Escape Codes
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/AT90USB_64x_128x/Serial_Stream.h> // Serial stream driver for the USB1287
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>                // Bicolour LEDs driver for the USBKEY
		#include <MyUSB/Scheduler/Scheduler.h>                    // Simple scheduler for task management
		
	/* Macros: */
		#define SIMAGE_CLASS                   0x06
		#define SIMAGE_SUBCLASS                0x01
		#define SIMAGE_PROTOCOL                0x01
		
		#define MAX_CONFIG_DESCRIPTOR_SIZE     512
	
	/* Enums: */
		enum
		{
			ControlError            = 0,
			DescriptorTooLarge      = 1,
			InterfaceNotFound       = 2,
			NoEndpointFound         = 3,
			SuccessfulConfigRead    = 4,
		} GetConfigDescriptorDataCodes_t;

	/* Task Definitions: */
		TASK(USB_SImage_Host);

	/* Event Handlers: */
		HANDLES_EVENT(USB_DeviceAttached);
		HANDLES_EVENT(USB_DeviceUnattached);
		HANDLES_EVENT(USB_HostError);
		HANDLES_EVENT(USB_DeviceEnumerationFailed);
		
	/* Function Prototypes: */
		uint8_t GetConfigDescriptorData(void);
		
#endif
