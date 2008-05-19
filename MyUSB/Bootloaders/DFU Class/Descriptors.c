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
		
	USBSpecification:       0x0101,
	Class:                  0x00,
	SubClass:               0x00,
	Protocol:               0x00,
				
	Endpoint0Size:          CONTROL_ENDPOINT_SIZE,
		
	VendorID:               0x03EB,
	ProductID:              PRODUCT_ID_CODE,
	ReleaseNumber:          0x0000,
		
	ManafacturerStrIndex:   0x01,
	ProductStrIndex:        0x02,
	SerialNumStrIndex:      0x03,
		
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

USB_Descriptor_Language_t LanguageString =
{
	Header:                 {Size: sizeof(USB_Descriptor_Language_t), Type: DTYPE_String},
		
	LanguageID:             LANGUAGE_ID_ENG
};

USB_Descriptor_String_t ManafacturerString =
{
	Header:                 {Size: USB_STRING_LEN(11), Type: DTYPE_String},
		
	UnicodeString:          {'D','e','a','n',' ','C','a','m','e','r','a'}
};

USB_Descriptor_String_t ProductString =
{
	Header:                 {Size: USB_STRING_LEN(16), Type: DTYPE_String},
		
	UnicodeString:          {'M','y','U','S','B',' ','B','o','o','t','l','o','a','d','e','r'}
};

USB_Descriptor_String_t SerialNumberString =
{
	Header:                 {Size: USB_STRING_LEN(5), Type: DTYPE_String},
		
	UnicodeString:          {'1','.',('0'+  BOOTLOADER_VERSION_MINOR),'.',('0'+  BOOTLOADER_VERSION_REV)}
};

bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index,
                       void** const DescriptorAddr, uint16_t* const Size)
{
	void*    DescriptorAddress = NULL;
	uint16_t DescriptorSize    = 0;

	switch (Type)
	{
		case DTYPE_Device:
			DescriptorAddress = DESCRIPTOR_ADDRESS(DeviceDescriptor);
			DescriptorSize    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			DescriptorAddress = DESCRIPTOR_ADDRESS(ConfigurationDescriptor);
			DescriptorSize    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (Index)
			{
				case 0x00:
					DescriptorAddress = DESCRIPTOR_ADDRESS(LanguageString);
					DescriptorSize    = sizeof(USB_Descriptor_Language_t);
					break;
				case 0x01:
					DescriptorAddress = DESCRIPTOR_ADDRESS(ManafacturerString);
					DescriptorSize    = ManafacturerString.Header.Size;
					break;
				case 0x02:
					DescriptorAddress = DESCRIPTOR_ADDRESS(ProductString);
					DescriptorSize    = ProductString.Header.Size;
					break;
				case 0x03:
					DescriptorAddress = DESCRIPTOR_ADDRESS(SerialNumberString);
					DescriptorSize    = SerialNumberString.Header.Size;
					break;
			}
			
			break;
	}
	
	if (DescriptorAddress != NULL)
	{
		*DescriptorAddr = DescriptorAddress;
		*Size           = DescriptorSize;

		return true;
	}
		
	return false;
}
