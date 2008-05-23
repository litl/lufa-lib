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

		#include <avr/pgmspace.h>

	/* Macros: */
		#define CDC_FUNCTIONAL_DESCRIPTOR(size)                      \
		     struct                                                  \
		     {                                                       \
		          USB_Descriptor_CDCFunctional_Header_t FuncHeader;  \
		          uint8_t                               Data[size];  \
		     }

		#define CDC_NOTIFICATION_EPNUM         3
		#define CDC_TX_EPNUM                   1	
		#define CDC_RX_EPNUM                   2	
		#define CDC_NOTIFICATION_EPSIZE        8
		#define CDC_TXRX_EPSIZE                16	

	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Header_t               Header;
			uint8_t                               SubType;
		} USB_Descriptor_CDCFunctional_Header_t;

		typedef struct
		{
			USB_Descriptor_Configuration_Header_t    Config;
			USB_Descriptor_Interface_t               CCI_Interface;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_IntHeader;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_CallManagement;
			CDC_FUNCTIONAL_DESCRIPTOR(1)             CDC_Functional_AbstractControlManagement;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_Union;
			USB_Descriptor_Endpoint_t                ManagementEndpoint;
			USB_Descriptor_Interface_t               DCI_Interface;
			USB_Descriptor_Endpoint_t                DataOutEndpoint;
			USB_Descriptor_Endpoint_t                DataInEndpoint;
		} USB_Descriptor_Configuration_t;

	/* Function Prototypes: */
		bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index, const uint16_t LanguageID,
		                       void** const DescriptorAddr, uint16_t* const DescriptorSize)
		                       ATTR_WARN_UNUSED_RESULT ATTR_WEAK ATTR_NON_NULL_PTR_ARG(4, 5);

#endif
