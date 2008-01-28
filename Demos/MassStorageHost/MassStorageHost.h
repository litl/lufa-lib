/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _MASS_STORE_HOST_H_
#define _MASS_STORE_HOST_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#include <stdio.h>

		#include "MassStoreCommands.h"

		#include <MyUSB/Common/ButtLoadTag.h>                     // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>             // ANSI Terminal Escape Codes
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/AT90USB_64x_128x/Serial_Stream.h> // Serial stream driver for the USB1287
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>                // Bicolour LEDs driver for the USBKEY
		#include <MyUSB/Scheduler/Scheduler.h>                    // Simple scheduler for task management
		
	/* Macros */
		#define MASS_STORE_CLASS                 0x08
		#define MASS_STORE_SUBCLASS              0x06
		#define MASS_STORE_PROTOCOL              0x50
		
		#define MAX_CONFIG_DESCRIPTOR_SIZE       512

	/* Enums */
		enum
		{
			ControlError         = 0,
			DescriptorTooLarge   = 1,
			InterfaceNotFound    = 2,
			NoEndpointFound      = 5,
			SuccessfulConfigRead = 6,
		} GetConfigDescriptorDataCodes_t;

	/* Task Definitions: */
		TASK(USB_MassStore_Host);

	/* Event Handlers: */
		HANDLES_EVENT(USB_DeviceAttached);
		HANDLES_EVENT(USB_DeviceUnattached);
		HANDLES_EVENT(USB_HostError);
		HANDLES_EVENT(USB_DeviceEnumerationFailed);
		
	/* External Variables: */
		extern uint16_t MassStoreEndpointSize_IN;
		extern uint16_t MassStoreEndpointSize_OUT;
		
	/* Function Prototypes: */
		void    ShowDiskReadError(uint8_t ErrorCode);
		uint8_t GetConfigDescriptorData(void);
		
#endif
