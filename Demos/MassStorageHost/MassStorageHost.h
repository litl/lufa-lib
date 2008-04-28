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
		#include <avr/wdt.h>
		#include <avr/pgmspace.h>
		#include <stdio.h>

		#include "MassStoreCommands.h"

		#include <MyUSB/Version.h>                                // Library Version Information
		#include <MyUSB/Common/ButtLoadTag.h>                     // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>             // ANSI Terminal Escape Codes
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/AT90USBXXX/Serial_Stream.h>       // Serial stream driver
		#include <MyUSB/Drivers/Board/LEDs.h>                     // LEDs driver
		#include <MyUSB/Scheduler/Scheduler.h>                    // Simple scheduler for task management
		
	/* Macros: */
		#define MASS_STORE_CLASS                 0x08
		#define MASS_STORE_SUBCLASS              0x06
		#define MASS_STORE_PROTOCOL              0x50
		
		#define MAX_CONFIG_DESCRIPTOR_SIZE       512

	/* Enums: */
		enum GetConfigDescriptorDataCodes_t
		{
			ControlError         = 0,
			DescriptorTooLarge   = 1,
			NoInterfaceFound     = 2,
			NoEndpointFound      = 3,
			SuccessfulConfigRead = 4,
		};

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
