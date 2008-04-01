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
		
		#define CONTROL_ENDPOINT_SIZE             32

		#if (defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1287__))
			#define PRODUCT_ID_CODE               0x2FFB
		#elif (defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB647__))
			#define PRODUCT_ID_CODE               0x2FF9
		#elif defined(__AVR_AT90USB162__)
			#define PRODUCT_ID_CODE               0x2FFA
		#elif defined(__AVR_AT90USB82__)
			#define PRODUCT_ID_CODE               0x2FF7
		#elif defined(__AVR_ATmega32U4__)
			#define PRODUCT_ID_CODE               0x2FF4
		#endif
		
		#if !defined(PRODUCT_ID_CODE)
			#error Current AVR model is not supported by this bootloader.
		#endif
	
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
