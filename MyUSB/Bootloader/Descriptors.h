/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <MyUSB/Drivers/USB/USB.h>
		
		#include "Bootloader.h"

	/* Macros: */
		#define DTYPE_DFUFunctional               0x21
		
		#define ATTR_WILL_DETATCH                 (1 << 3)
		#define ATTR_MANEFESTATION_TOLLERANT      (1 << 2)
		#define ATTR_CAN_UPLOAD                   (1 << 1)
		#define ATTR_CAN_DOWNLOAD                 (1 << 0)

	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Header_t               Header;
			
			uint8_t                               Attributes;
			uint16_t                              DetatchTimeout;
			uint16_t                              TransferSize;
			
			uint16_t                              DFUSpecification;			
		} USB_DFU_Functional_Descriptor_t;
	
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            DFUInterface;
			USB_DFU_Functional_Descriptor_t       DFUFunctional;
		} USB_Descriptor_Configuration_t;
		
	/* External Variables: */
		extern USB_Descriptor_Configuration_t ConfigurationDescriptor;
		
	/* Function Prototypes: */
		bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index,
							   void** const DescriptorAddr, uint16_t* const Size)
							   ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3, 4);

#endif
