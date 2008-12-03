/*
             LUFA Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _CONFIGDESCRIPTOR_H_
#define _CONFIGDESCRIPTOR_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/USB.h>                        // USB Functionality
		#include <LUFA/Drivers/USB/Class/ConfigDescriptor.h>     // Configuration Descriptor Parser
		
		#include "HIDReport.h"
		
	/* Macros: */
		#define KEYBOARD_DATAPIPE              1
		#define KEYBOARD_CLASS                 0x03
		#define KEYBOARD_PROTOCOL              0x01
		
		#define DTYPE_HID                      0x21
		#define DTYPE_Report                   0x22

		#define MAX_CONFIG_DESCRIPTOR_SIZE     512
	
	/* Enums: */
		enum KeyboardHostWithParser_GetConfigDescriptorDataCodes_t
		{
			SuccessfulConfigRead               = 0,
			ControlError                       = 1,
			DescriptorTooLarge                 = 2,
			InvalidConfigDataReturned          = 3,
			NoHIDInterfaceFound                = 4,
			NoHIDDescriptorFound               = 5,
			NoEndpointFound                    = 6,
		};
	
	/* Configuration Descriptor Comparison Functions: */
		DESCRIPTOR_COMPARATOR(NextKeyboardInterface);
		DESCRIPTOR_COMPARATOR(NextInterfaceKeyboardDataEndpoint);
		DESCRIPTOR_COMPARATOR(NextHID);

	/* Function Prototypes: */
		uint8_t ProcessConfigurationDescriptor(void);	

#endif
