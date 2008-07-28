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
		
	ManufacturerStrIndex:   NO_DESCRIPTOR_STRING,
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

bool USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex,
                       void** const DescriptorAddress, uint16_t* const DescriptorSize)
{
	void*    Address = NULL;
	uint16_t Size    = 0;

	switch (wValue >> 8)
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
			if (!(wValue))
			{
				Address = DESCRIPTOR_ADDRESS(LanguageString);
				Size    = LanguageString.Header.Size;
			}
			else
			{
				Address = DESCRIPTOR_ADDRESS(ProductString);
				Size    = ProductString.Header.Size;
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
