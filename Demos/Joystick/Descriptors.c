/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Descriptors.h"

USB_Descriptor_HIDReport_Datatype_t JoystickReport[] PROGMEM =
{
	0x05, 0x01,          /* Usage Page (Generic Desktop)                       */
	0x09, 0x04,          /* Usage (Joystick)                                   */
	0xa1, 0x01,          /* Collection (Application)                           */
	0x09, 0x01,          /*   Usage (Pointer)                                  */
	0xa1, 0x00,          /*   Collection (Physical)                            */
	0x05, 0x01,          /*     Usage Page (Generic Desktop)                   */
	0x09, 0x30,          /*     Usage (X)                                      */
	0x09, 0x31,          /*     Usage (Y)                                      */
	0x15, 0x9c,          /*     Logical Minimum (-100)                         */
	0x25, 0x64,          /*     Logical Maximum (100)                          */
	0x75, 0x08,          /*     Report Size (8)                                */
	0x95, 0x02,          /*     Report Count (2)                               */
	0x81, 0x82,          /*     Input (Data, Variable, Absolute, Volatile)     */
	0x05, 0x09,          /*     Usage Page (Button)                            */
	0x09, 0x02,          /*     Usage (Button 2)                               */
	0x09, 0x01,          /*     Usage (Button 1)                               */
	0x15, 0x00,          /*     Logical Minimum (0)                            */
	0x25, 0x01,          /*     Logical Maximum (1)                            */
	0x75, 0x01,          /*     Report Size (1)                                */
	0x95, 0x02,          /*     Report Count (2)                               */
	0x81, 0x02,          /*     Input (Data, Variable, Absolute)               */
	0x75, 0x06,          /*     Report Size (6)                                */
	0x95, 0x01,          /*     Report Count (1)                               */
	0x81, 0x01,          /*     Input (Constant)                               */
	0xc0,                /*   End Collection                                   */
	0xc0                 /* End Collection                                     */
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
	ProductID:              0x2043,
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
			Protocol:               0x00,
				
			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},

	JoystickHID:
		{
			Header:                 {Size: sizeof(USB_Descriptor_HID_t), Type: DTYPE_HID},
									 
			HIDSpec:          		0x1001,
			CountryCode:      		0x00,
			TotalHIDDescriptors:    0x01,
			HIDReportType:    		0x22,
			HIDReportLength:        sizeof(JoystickReport)
		},

	JoystickEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | JOYSTICK_EPNUM),
			Attributes:       		EP_TYPE_INTERRUPT,
			EndpointSize:           JOYSTICK_EPSIZE,
			PollingIntervalMS:		0x02
		}	
};

USB_Descriptor_String_t LanguageString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(1), Type: DTYPE_String},
		
	UnicodeString:          {LANGUAGE_ID_ENG}
};

USB_Descriptor_String_t ManafacturerString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(11), Type: DTYPE_String},
		
	UnicodeString:          L"Dean Camera"
};

USB_Descriptor_String_t ProductString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(19), Type: DTYPE_String},
		
	UnicodeString:          L"MyUSB Joystick Demo"
};

USB_Descriptor_String_t SerialNumberString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(12), Type: DTYPE_String},
		
	UnicodeString:          L"000000000000"
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
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = DESCRIPTOR_ADDRESS(ManafacturerString);
					Size    = pgm_read_byte(&ManafacturerString.Header.Size);
					break;
				case 0x02:
					Address = DESCRIPTOR_ADDRESS(ProductString);
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
				case 0x03:
					Address = DESCRIPTOR_ADDRESS(SerialNumberString);
					Size    = pgm_read_byte(&SerialNumberString.Header.Size);
					break;
			}
			
			break;
		case DTYPE_HID:
			Address = DESCRIPTOR_ADDRESS(ConfigurationDescriptor.JoystickHID);
			Size    = sizeof(USB_Descriptor_HID_t);
			break;
		case DTYPE_Report:
			Address = DESCRIPTOR_ADDRESS(JoystickReport);
			Size    = sizeof(JoystickReport);
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
