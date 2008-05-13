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
		
		#include "MassStorageHost.h"
		
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

	/* Configuration Descriptor Comparison Functions: */
		DESCRIPTOR_COMPARATOR(NextMassStorageInterface);
		DESCRIPTOR_COMPARATOR(NextInterfaceBulkDataEndpoint);

	/* Function Prototypes: */
		uint8_t ProcessConfigurationDescriptor(void);	

#endif
