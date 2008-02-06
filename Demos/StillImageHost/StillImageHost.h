/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _KEYBOARD_HOST_H_
#define _KEYBOARD_HOST_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#include <stdio.h>

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
		
		#define SIMAGE_DATA_IN_PIPE            0x01
		#define SIMAGE_DATA_OUT_PIPE           0x02
		#define SIMAGE_EVENTS_PIPE             0x03
	
		#define MAX_CONFIG_DESCRIPTOR_SIZE     512
	
	/* Type Defines: */
		typedef struct
		{
			uint32_t Length;
			uint16_t Type;
			uint16_t Code;
			uint32_t TransactionID;
		} PIMA_Container_Header_t;
	
	/* Enums: */
		enum
		{
			CType_Undefined         = 0,
			CType_CommandBlock      = 1,
			CType_DataBlock         = 2,
			CType_ResponseBlock     = 3,
			CType_EventBlock        = 4,
		} PIMA_Container_Types_t;
	
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
