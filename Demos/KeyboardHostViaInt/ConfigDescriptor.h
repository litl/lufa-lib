/*
             MyUSB Library
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
		#include <MyUSB/Drivers/USB/USB.h>                        // USB Functionality
		#include <MyUSB/Drivers/USB/Class/ConfigDescriptor.h>     // Configuration Descriptor Parser
		
		#include "KeyboardHostViaInt.h"
		
	/* Macros: */
		#define KEYBOARD_CLASS                 0x03
		#define KEYBOARD_PROTOCOL              0x01

		#define MAX_CONFIG_DESCRIPTOR_SIZE     512
	
	/* Enums: */
		enum GetConfigDescriptorDataCodes_t
		{
			SuccessfulConfigRead               = 0,
			ControlError                       = 1,
			DescriptorTooLarge                 = 2,
			InvalidConfigDataReturned          = 3,
			NoHIDInterfaceFound                = 4,
			NoEndpointFound                    = 5,
		};
	
	/* Configuration Descriptor Comparison Functions: */
		DESCRIPTOR_COMPARATOR(NextKeyboardInterface);
		DESCRIPTOR_COMPARATOR(NextInterfaceKeyboardDataEndpoint);
		
	/* Function Prototypes: */
		uint8_t ProcessConfigurationDescriptor(void);	

#endif
