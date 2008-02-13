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

		#define CDC1_NOTIFICATION_EPNUM         3
		#define CDC1_TX_EPNUM                   1	
		#define CDC1_RX_EPNUM                   2	
		#define CDC2_NOTIFICATION_EPNUM         6
		#define CDC2_TX_EPNUM                   4	
		#define CDC2_RX_EPNUM                   5	
		#define CDC_NOTIFICATION_EPSIZE         8
		#define CDC_TXRX_EPSIZE                 64	

	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Header_t               Header;
			uint8_t                               SubType;
		} USB_Descriptor_CDCFunctional_Header_t;

		typedef struct
		{
			USB_Descriptor_Configuration_Header_t    Config;

			USB_Descriptor_Interface_t               CCI_Interface1;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_IntHeader1;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_CallManagement1;
			CDC_FUNCTIONAL_DESCRIPTOR(1)             CDC_Functional_AbstractControlManagement1;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_Union1;
			USB_Descriptor_Endpoint_t                ManagementEndpoint1;
			USB_Descriptor_Interface_t               DCI_Interface1;
			USB_Descriptor_Endpoint_t                DataOutEndpoint1;
			USB_Descriptor_Endpoint_t                DataInEndpoint1;
			
			USB_Descriptor_Interface_t               CCI_Interface2;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_IntHeader2;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_CallManagement2;
			CDC_FUNCTIONAL_DESCRIPTOR(1)             CDC_Functional_AbstractControlManagement2;
			CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_Union2;
			USB_Descriptor_Endpoint_t                ManagementEndpoint2;
			USB_Descriptor_Interface_t               DCI_Interface2;
			USB_Descriptor_Endpoint_t                DataOutEndpoint2;
			USB_Descriptor_Endpoint_t                DataInEndpoint2;			
		} USB_Descriptor_Configuration_t;

	/* Function Prototypes: */
		bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index,
							   void** const DescriptorAddr, uint16_t* const Size)
							   ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3, 4);

#endif
