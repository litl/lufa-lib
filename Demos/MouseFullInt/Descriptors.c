/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Descriptors.h"

USB_Descriptor_HID_Mouse_Report_t MouseReport PROGMEM =
{
	ReportData:
	{
		0x05, 0x01,          /* Usage Page (Generic Desktop)             */
		0x09, 0x02,          /* Usage (Mouse)                            */
		0xA1, 0x01,          /* Collection (Application)                 */
		0x09, 0x01,          /*   Usage (Pointer)                        */
		0xA1, 0x00,          /*   Collection (Application)               */
		0x05, 0x09,          /*     Usage Page (Button)                  */
		0x19, 0x01,          /*     Usage Minimum (1)                    */
		0x29, 0x02,          /*     Usage Maximum (2)                    */
		0x15, 0x00,          /*     Logical Minimum (0)                  */
		0x25, 0x01,          /*     Logical Maximum (1)                  */
		0x75, 0x01,          /*     Report Size (1)                      */
		0x95, 0x02,          /*     Report Count (2)                     */
		0x81, 0x02,          /*     Input (Data, Variable, Absolute)     */
		0x75, 0x06,          /*     Report Size (6)                      */
		0x95, 0x01,          /*     Report Count (1)                     */
		0x81, 0x01,          /*     Input (Constant)                     */
		0x05, 0x01,          /*     Usage Page (Generic Desktop Control) */
		0x09, 0x30,          /*     Usage X                              */
		0x09, 0x31,          /*     Usage Y                              */
		0x09, 0x38,          /*     Usage Wheel                          */
		0x15, 0x81,          /*     Logical Minimum (-127)               */
		0x25, 0x7F,          /*     Logical Maximum (127)                */
		0x75, 0x08,          /*     Report Size (8)                      */
		0x95, 0x02,          /*     Report Count (2)                     */
		0x81, 0x06,          /*     Input (Data, Variable, Relative)     */
		0xC0,                /*   End Collection                         */
		0xC0                 /* End Collection                           */
	}
};

USB_Descriptor_Device_t DeviceDescriptor PROGMEM =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       0x0101,
	Class:                  0x00,
	SubClass:               0x00,
	Protocol:               0x00,
				
	Endpoint0Size:          8,
		
	VendorID:               0x03EB,
	ProductID:              0x2041,
	ReleaseNumber:          0x0000,
		
	ManafacturerStrIndex:   0x01,
	ProductStrIndex:        0x02,
	SerialNumStrIndex:      0x03,
		
	NumberOfConfigurations: 1
};
	
USB_Descriptor_Configuration_t ConfigurationDescriptor PROGMEM =
{
	Config:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Configuration_Header_t), Type: DTYPE_Configuration},

			TotalConfigurationSize: sizeof(USB_Descriptor_Configuration_t),
			TotalInterfaces:        1,
				
			ConfigurationNumber:    1,
			ConfigurationStrIndex:  NO_DESCRIPTOR_STRING,
				
			ConfigAttributes:       (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
			
			MaxPowerConsumption:    USB_CONFIG_POWER_MA(100)
		},
		
	Interface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        0x00,
			AlternateSetting:       0x00,
			
			TotalEndpoints:         1,
				
			Class:                  0x03,
			SubClass:               0x00,
			Protocol:               0x02,
				
			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},

	MouseHID:
		{
			Header:                 {Size: sizeof(USB_Descriptor_HID_t), Type: DTYPE_HID},
									 
			HIDSpec:          		0x1001,
			CountryCode:      		0x00,
			TotalHIDDescriptors:    0x01,
			HIDReportType:    		0x22,
			HIDReportLength:        sizeof(USB_Descriptor_HID_Mouse_Report_t)
		},

	MouseEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | MOUSE_EPNUM),
			Attributes:       		EP_TYPE_INTERRUPT,
			EndpointSize:           MOUSE_EPSIZE,
			PollingIntervalMS:		0x02
		}	
};

USB_Descriptor_Language_t LanguageString PROGMEM =
{
	Header:                 {Size: sizeof(USB_Descriptor_Language_t), Type: DTYPE_String},
		
	LanguageID:             LANGUAGE_ID_ENG
};

USB_Descriptor_String_t ManafacturerString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(11), Type: DTYPE_String},
		
	UnicodeString:          {'D','e','a','n',' ','C','a','m','e','r','a'}
};

USB_Descriptor_String_t ProductString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(16), Type: DTYPE_String},
		
	UnicodeString:          {'M','y','U','S','B',' ','M','o','u','s','e',' ','D','e','m','o'}
};

USB_Descriptor_String_t SerialNumberString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(12), Type: DTYPE_String},
		
	UnicodeString:          {'0','0','0','0','0','0','0','0','0','0','0','0'}
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
					DescriptorSize    = pgm_read_byte(&ManafacturerString.Header.Size);
					break;
				case 0x02:
					DescriptorAddress = DESCRIPTOR_ADDRESS(ProductString);
					DescriptorSize    = pgm_read_byte(&ProductString.Header.Size);
					break;
				case 0x03:
					DescriptorAddress = DESCRIPTOR_ADDRESS(SerialNumberString);
					DescriptorSize    = pgm_read_byte(&SerialNumberString.Header.Size);
					break;
			}
			
			break;
		case DTYPE_HID:
			DescriptorAddress = DESCRIPTOR_ADDRESS(ConfigurationDescriptor.MouseHID);
			DescriptorSize    = sizeof(USB_Descriptor_HID_t);
			break;
		case DTYPE_Report:
			DescriptorAddress = DESCRIPTOR_ADDRESS(MouseReport);
			DescriptorSize    = sizeof(USB_Descriptor_HID_Mouse_Report_t);
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

