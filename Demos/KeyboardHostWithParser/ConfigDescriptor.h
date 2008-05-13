/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _CONFIGDESCRIPTOR_H_
#define _CONFIGDESCRIPTOR_H_

	/* Includes: */
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/USB/Class/ConfigDescriptor.h>     // Configuration Descriptor Parser
		
		#include "KeyboardHostWithParser.h"
		
	/* Macros: */
		#define KEYBOARD_DATAPIPE              1
		#define KEYBOARD_CLASS                 0x03
		#define KEYBOARD_PROTOCOL              0x01
		
		#define DTYPE_HID                      0x21
		#define DTYPE_Report                   0x22

		#define MAX_CONFIG_DESCRIPTOR_SIZE     512
	
	/* Enums: */
		enum GetConfigDescriptorDataCodes_t
		{
			ControlError         = 0,
			DescriptorTooLarge   = 1,
			NoHIDInterfaceFound  = 2,
			NoHIDDescriptorFound = 3,
			NoEndpointFound      = 4,
			SuccessfulConfigRead = 5,
		};
	
	/* Configuration Descriptor Comparison Functions: */
		DESCRIPTOR_COMPARATOR(NextKeyboardInterface);
		DESCRIPTOR_COMPARATOR(NextInterfaceKeyboardDataEndpoint);
		DESCRIPTOR_COMPARATOR(NextHID);

	/* Function Prototypes: */
		uint8_t ProcessConfigurationDescriptor(void);	

#endif
