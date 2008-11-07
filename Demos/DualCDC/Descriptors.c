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

USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       VERSION_BCD(01.10),
	Class:                  0xEF,
	SubClass:               0x02,
	Protocol:               0x01,
				
	Endpoint0Size:          8,
		
	VendorID:               0x03EB,
	ProductID:              0x204E,
	ReleaseNumber:          0x0000,
		
	ManufacturerStrIndex:   0x01,
	ProductStrIndex:        0x02,
	SerialNumStrIndex:      NO_DESCRIPTOR_STRING,
		
	NumberOfConfigurations: 1
};
	
USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	Config:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Configuration_Header_t), Type: DTYPE_Configuration},

			TotalConfigurationSize: sizeof(USB_Descriptor_Configuration_t),
			TotalInterfaces:        4,
				
			ConfigurationNumber:    1,
			ConfigurationStrIndex:  NO_DESCRIPTOR_STRING,
				
			ConfigAttributes:       (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
			
			MaxPowerConsumption:    USB_CONFIG_POWER_MA(100)
		},
		
	IAD1:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_Association_t), Type: DTYPE_InterfaceAssociation},

			FirstInterfaceIndex:   0,
			TotalInterfaces:        2,

			Class:                  0x02,
			SubClass:               0x02,
			Protocol:               0x01,

			IADStrIndex:            NO_DESCRIPTOR_STRING
		},

	CDC1_CCI_Interface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        0,
			AlternateSetting:       0,
			
			TotalEndpoints:         1,
				
			Class:                  0x02,
			SubClass:               0x02,
			Protocol:               0x01,
				
			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},

	CDC1_Functional_IntHeader:
		{
			Header:                 {Size: sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)), Type: 0x24},
			SubType:                0x00,
			
			Data:                   {0x01, 0x10}
		},

	CDC1_Functional_CallManagement:
		{
			Header:                 {Size: sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)), Type: 0x24},
			SubType:                0x01,
			
			Data:                   {0x03, 0x01}
		},

	CDC1_Functional_AbstractControlManagement:
		{
			Header:                 {Size: sizeof(CDC_FUNCTIONAL_DESCRIPTOR(1)), Type: 0x24},
			SubType:                0x02,
			
			Data:                   {0x06}
		},
		
	CDC1_Functional_Union:
		{
			Header:                 {Size: sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)), Type: 0x24},
			SubType:                0x06,
			
			Data:                   {0x00, 0x01}
		},

	CDC1_ManagementEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | CDC1_NOTIFICATION_EPNUM),
			Attributes:       		EP_TYPE_INTERRUPT,
			EndpointSize:           CDC_NOTIFICATION_EPSIZE,
			PollingIntervalMS:		0xFF
		},

	CDC1_DCI_Interface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        1,
			AlternateSetting:       0,
			
			TotalEndpoints:         2,
				
			Class:                  0x0A,
			SubClass:               0x00,
			Protocol:               0x00,
				
			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},

	CDC1_DataOutEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_OUT | CDC1_RX_EPNUM),
			Attributes:       		EP_TYPE_BULK,
			EndpointSize:           CDC_TXRX_EPSIZE,
			PollingIntervalMS:		0x00
		},
		
	CDC1_DataInEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | CDC1_TX_EPNUM),
			Attributes:       		EP_TYPE_BULK,
			EndpointSize:           CDC_TXRX_EPSIZE,
			PollingIntervalMS:		0x00
		},

	IAD2:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_Association_t), Type: DTYPE_InterfaceAssociation},

			FirstInterfaceIndex:    2,
			TotalInterfaces:        2,

			Class:                  0x02,
			SubClass:               0x02,
			Protocol:               0x01,

			IADStrIndex:            NO_DESCRIPTOR_STRING
		},

	CDC2_CCI_Interface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        2,
			AlternateSetting:       0,
			
			TotalEndpoints:         1,
				
			Class:                  0x02,
			SubClass:               0x02,
			Protocol:               0x01,
				
			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},

	CDC2_Functional_IntHeader:
		{
			Header:                 {Size: sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)), Type: 0x24},
			SubType:                0x00,
			
			Data:                   {0x01, 0x10}
		},

	CDC2_Functional_CallManagement:
		{
			Header:                 {Size: sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)), Type: 0x24},
			SubType:                0x01,
			
			Data:                   {0x03, 0x03}
		},

	CDC2_Functional_AbstractControlManagement:
		{
			Header:                 {Size: sizeof(CDC_FUNCTIONAL_DESCRIPTOR(1)), Type: 0x24},
			SubType:                0x02,
			
			Data:                   {0x06}
		},
		
	CDC2_Functional_Union:
		{
			Header:                 {Size: sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)), Type: 0x24},
			SubType:                0x06,
			
			Data:                   {0x02, 0x03}
		},

	CDC2_ManagementEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | CDC2_NOTIFICATION_EPNUM),
			Attributes:       		EP_TYPE_INTERRUPT,
			EndpointSize:           CDC_NOTIFICATION_EPSIZE,
			PollingIntervalMS:		0xFF
		},

	CDC2_DCI_Interface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        3,
			AlternateSetting:       0,
			
			TotalEndpoints:         2,
				
			Class:                  0x0A,
			SubClass:               0x00,
			Protocol:               0x00,
				
			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},

	CDC2_DataOutEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_OUT | CDC2_RX_EPNUM),
			Attributes:       		EP_TYPE_BULK,
			EndpointSize:           CDC_TXRX_EPSIZE,
			PollingIntervalMS:		0x00
		},
		
	CDC2_DataInEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | CDC2_TX_EPNUM),
			Attributes:       		EP_TYPE_BULK,
			EndpointSize:           CDC_TXRX_EPSIZE,
			PollingIntervalMS:		0x00
		}
};

USB_Descriptor_String_t PROGMEM LanguageString =
{
	Header:                 {Size: USB_STRING_LEN(1), Type: DTYPE_String},
		
	UnicodeString:          {LANGUAGE_ID_ENG}
};

USB_Descriptor_String_t PROGMEM ManufacturerString =
{
	Header:                 {Size: USB_STRING_LEN(11), Type: DTYPE_String},
		
	UnicodeString:          L"Dean Camera"
};

USB_Descriptor_String_t PROGMEM ProductString =
{
	Header:                 {Size: USB_STRING_LEN(14), Type: DTYPE_String},
		
	UnicodeString:          L"MyUSB Dual CDC Demo"
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
			switch (wValue & 0xFF)
			{
				case 0x00:
					Address = DESCRIPTOR_ADDRESS(LanguageString);
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = DESCRIPTOR_ADDRESS(ManufacturerString);
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = DESCRIPTOR_ADDRESS(ProductString);
					Size    = pgm_read_byte(&ProductString.Header.Size);
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
