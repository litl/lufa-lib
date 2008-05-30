/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Descriptors.h"

USB_Descriptor_Device_t DeviceDescriptor =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       VERSION_BCD(01.10),
	Class:                  0x00,
	SubClass:               0x00,
	Protocol:               0x00,
				
	Endpoint0Size:          CONTROL_ENDPOINT_SIZE,
		
	VendorID:               0x03EB,
	ProductID:              PRODUCT_ID_CODE,
	ReleaseNumber:          0x0000,
		
	ManafacturerStrIndex:   NO_DESCRIPTOR_STRING,
	ProductStrIndex:        0x01,
	SerialNumStrIndex:      NO_DESCRIPTOR_STRING,
		
	NumberOfConfigurations: 1
};
	
USB_Descriptor_Configuration_t ConfigurationDescriptor =
{
	Config:
		{
			Header:                   {Size: sizeof(USB_Descriptor_Configuration_Header_t), Type: DTYPE_Configuration},

			TotalConfigurationSize:   sizeof(USB_Descriptor_Configuration_t),
			TotalInterfaces:          1,

			ConfigurationNumber:      1,
			ConfigurationStrIndex:    NO_DESCRIPTOR_STRING,
				
			ConfigAttributes:         (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
			
			MaxPowerConsumption:      USB_CONFIG_POWER_MA(100)
		},
		
	DFUInterface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        0,
			AlternateSetting:       0,
			
			TotalEndpoints:         0,
				
			Class:                  0xFE,
			SubClass:               0x01,
			Protocol:               0x02,

			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},
		
	DFUFunctional:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Header_t), Type: DTYPE_DFUFunctional},
			
			Attributes:             (ATTR_CAN_UPLOAD | ATTR_CAN_DOWNLOAD),

			DetatchTimeout:         0x0000,
			TransferSize:           0x0c00,
		
			DFUSpecification:       0x0101
		}
};

USB_Descriptor_String_t LanguageString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(1), Type: DTYPE_String},
		
	UnicodeString:          {LANGUAGE_ID_ENG}
};

USB_Descriptor_String_t ProductString =
{
	Header:                 {Size: USB_STRING_LEN(20), Type: DTYPE_String},
		
	UnicodeString:          L"MyUSB DFU Bootloader"
};

bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index, const uint16_t LanguageID,
					   void** const DescriptorAddress, uint16_t* const DescriptorSize)
{
	void*    Address = NULL;
	uint16_t Size    = 0;

	switch (Type)
	{
		case DTYPE_Device:
			Address = DESCRIPTOR_ADDRESS(DeviceDescriptor);
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = DESCRIPTOR_ADDRESS(ConfigurationDescriptor);
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (Index)
			{
				case 0x00:
					Address = DESCRIPTOR_ADDRESS(LanguageString);
					Size    = LanguageString.Header.Size;
					break;
				case 0x01:
					Address = DESCRIPTOR_ADDRESS(ProductString);
					Size    = ProductString.Header.Size;
					break;
			}
			
			break;
	}
	
	if (Address != NULL)
	{
		*DescriptorAddress = Address;
		*DescriptorSize    = Size;

		return true;
	}
		
	return false;
}
