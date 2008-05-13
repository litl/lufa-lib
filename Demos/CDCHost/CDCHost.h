/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _CDC_HOST_H_
#define _CDC_HOST_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/pgmspace.h>
		#include <stdio.h>

		#include <MyUSB/Version.h>                                // Library Version Information
		#include <MyUSB/Common/ButtLoadTag.h>                     // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/Misc/TerminalCodes.h>             // ANSI Terminal Escape Codes
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/USB/Class/ConfigDescriptor.h>     // Configuration Descriptor Parser
		#include <MyUSB/Drivers/AT90USBXXX/Serial_Stream.h>       // Serial stream driver
		#include <MyUSB/Drivers/Board/LEDs.h>                     // LEDs driver
		#include <MyUSB/Scheduler/Scheduler.h>                    // Simple scheduler for task management
		
	/* Macros: */
		#define MAX_CONFIG_DESCRIPTOR_SIZE     512
		
		#define CDC_CLASS                      0x02
		#define CDC_SUBCLASS                   0x02
		#define CDC_PROTOCOL                   0x01
		
		#define CDC_DATAPIPE_IN                1
		#define CDC_DATAPIPE_OUT               2
		#define CDC_NOTIFICATIONPIPE           3
		
	/* Enums: */
		enum GetConfigDescriptorDataCodes_t
		{
			ControlError         = 0,
			DescriptorTooLarge   = 1,
			NoCDCInterfaceFound  = 2,
			NoEndpointFound      = 3,
			SuccessfulConfigRead = 4,
		};

	/* Task Definitions: */
		TASK(USB_CDC_Host);

	/* Configuration Descriptor Comparison Functions: */
		DESCRIPTOR_COMPARATOR(NextCDCInterface);
		DESCRIPTOR_COMPARATOR(NextInterfaceCDCDataEndpoint);

	/* Event Handlers: */
		HANDLES_EVENT(USB_DeviceAttached);
		HANDLES_EVENT(USB_DeviceUnattached);
		HANDLES_EVENT(USB_HostError);
		HANDLES_EVENT(USB_DeviceEnumerationFailed);
		
	/* Function Prototypes: */
		uint8_t ProcessConfigurationDescriptor(void);
		
#endif
