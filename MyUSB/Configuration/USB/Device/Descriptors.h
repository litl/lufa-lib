/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

	/* Includes: */
		#include <avr/pgmspace.h>
		#include <stdbool.h>

		#include "../../../Drivers/USB/USB.h"
		#include "../../../Common/FunctionAttributes.h"

	/* Public Interface - May be used in end-application: */
		/* Macros */
			#define NO_DESCRIPTOR_STRING              0
			
			#define USB_CONFIG_POWER_MA(x)            (x >> 1)
			#define USB_STRING_LEN(x)                 (sizeof(USB_Descriptor_Header_t) + (x << 1))
			
			#define LANGUAGE_ID_ENG                   0x0409

			#define USB_CONFIG_ATTR_BUSPOWERED        0b10000000
			#define USB_CONFIG_ATTR_SELFPOWERED       0b11000000
			#define USB_CONFIG_ATTR_REMOTEWAKEUP      0b10100000

			// Descriptor Settings:
			#define CONFIGURATIONS                    1
			#define CONFIG_ATTRIBUTES                 (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED)

		/* Enums */
			enum DescriptorTypes
			{
				DTYPE_Device            = 1,
				DTYPE_Configuration     = 2,
				DTYPE_String            = 3,
				DTYPE_Interface         = 4,
				DTYPE_Endpoint          = 5,
				DTYPE_DeviceQualifier   = 6,
				DTYPE_Other             = 7,
				DTYPE_InterfacePower    = 8
			};

		/* Type Defines: */
			typedef struct
			{
				uint8_t Size;
				uint8_t Type;
			} USB_Descriptor_Header_t;
			
			typedef struct
			{
				USB_Descriptor_Header_t               Header;

				uint16_t                              USBSpecification;
				uint8_t                               Class;
				uint8_t                               SubClass;
				uint8_t                               Protocol;
				
				uint8_t                               Endpoint0Size;
				
				uint16_t                              VendorID;
				uint16_t                              ProductID;
				uint16_t                              ReleaseNumber;
				
				uint8_t                               ManafacturerStrIndex;
				uint8_t                               ProductStrIndex;
				uint8_t                               SerialNumStrIndex;

				uint8_t                               NumberOfConfigurations;
			} USB_Descriptor_Device_t;

			typedef struct
			{
				USB_Descriptor_Header_t               Header;
			
				uint16_t                              TotalConfigurationSize;
				uint8_t                               TotalInterfaces;

				uint8_t                               ConfigurationNumber;
				uint8_t                               ConfigurationStrIndex;
				
				uint8_t                               ConfigAttributes;
				
				uint8_t                               MaxPowerConsumption;
			} USB_Descriptor_Configuration_Header_t;
			
			typedef struct
			{
				USB_Descriptor_Header_t               Header;

				uint16_t                              USBSpecification;
				uint8_t                               Class;
				uint8_t                               SubClass;
				uint8_t                               Protocol;
				
				uint8_t                               Endpoint0Size;

			} USB_Descriptor_DeviceQualifier_t;

			typedef struct
			{
				USB_Descriptor_Header_t               Header;

				uint8_t                               InterfaceNumber;
				uint8_t                               AlternateSetting;
				uint8_t                               TotalEndpoints;
				
				uint8_t                               Class;
				uint8_t                               SubClass;
				uint8_t                               Protocol;

				uint8_t                               InterfaceStrIndex;
			} USB_Descriptor_Interface_t;

			typedef struct
			{
				USB_Descriptor_Header_t               Header;

				uint8_t                               EndpointAddress;
				uint8_t                               Attributes;
				
				uint16_t                              EndpointSize;
				
				uint8_t                               PollingIntervalMS;		
			} USB_Descriptor_Endpoint_t;

			typedef struct
			{
				USB_Descriptor_Header_t               Header;
				
				uint16_t                              UnicodeString[];
			} USB_Descriptor_String_t;

			typedef struct
			{
				USB_Descriptor_Header_t               Header;
				
				uint16_t                              LanguageID;
			} USB_Descriptor_Language_t;
			
			typedef	struct
			{
				USB_Descriptor_Configuration_Header_t Config;
				USB_Descriptor_Interface_t            Interface;

				// Add Interface and Endpoint descriptors as required to configuration descriptor here
			} USB_Descriptor_Configuration_t;

		/* Global Variables: */
			extern USB_Descriptor_Device_t            DeviceDescriptor;
			extern USB_Descriptor_Configuration_t     ConfigurationDescriptor;
			
		/* Function Prototypes: */
			bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index, void** DescriptorAddr, uint16_t* Size) ATTR_WARN_UNUSED_RESULT;
			
#endif
