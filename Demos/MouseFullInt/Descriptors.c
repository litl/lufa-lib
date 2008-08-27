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

USB_Descriptor_HIDReport_Datatype_t MouseReport[] PROGMEM =
{
	0x05, 0x01,          /* Usage Page (Generic Desktop)             */
	0x09, 0x02,          /* Usage (Mouse)                            */
	0xA1, 0x01,          /* Collection (Application)                 */
	0x09, 0x01,          /*   Usage (Pointer)                        */
	0xA1, 0x00,          /*   Collection (Application)               */
	0x95, 0x03,          /*     Report Count (3)                     */
	0x75, 0x01,          /*     Report Size (1)                      */
	0x05, 0x09,          /*     Usage Page (Button)                  */
	0x19, 0x01,          /*     Usage Minimum (Button 1)             */
	0x29, 0x03,          /*     Usage Maximum (Button 3)             */
	0x15, 0x00,          /*     Logical Minimum (0)                  */
	0x25, 0x01,          /*     Logical Maximum (1)                  */
	0x81, 0x02,          /*     Input (Data, Variable, Absolute)     */
	0x95, 0x01,          /*     Report Count (1)                     */
	0x75, 0x05,          /*     Report Size (5)                      */
	0x81, 0x01,          /*     Input (Constant)                     */
	0x75, 0x08,          /*     Report Size (8)                      */
	0x95, 0x02,          /*     Report Count (2)                     */
	0x05, 0x01,          /*     Usage Page (Generic Desktop Control) */
	0x09, 0x30,          /*     Usage X                              */
	0x09, 0x31,          /*     Usage Y                              */
	0x15, 0x81,          /*     Logical Minimum (-127)               */
	0x25, 0x7F,          /*     Logical Maximum (127)                */
	0x81, 0x06,          /*     Input (Data, Variable, Relative)     */
	0xC0,                /*   End Collection                         */
	0xC0                 /* End Collection                           */
};

USB_Descriptor_Device_t DeviceDescriptor PROGMEM =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       VERSION_BCD(01.10),
	Class:                  0x00,
	SubClass:               0x00,
	Protocol:               0x00,
				
	Endpoint0Size:          8,
		
	VendorID:               0x03EB,
	ProductID:              0x2041,
	ReleaseNumber:          0x0000,
		
	ManufacturerStrIndex:   0x01,
	ProductStrIndex:        0x02,
	SerialNumStrIndex:      NO_DESCRIPTOR_STRING,
		
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
			SubClass:               0x01,
			Protocol:               0x02,
				
			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},

	MouseHID:
		{
			Header:                 {Size: sizeof(USB_Descriptor_HID_t), Type: DTYPE_HID},
									 
			HIDSpec:          		VERSION_BCD(01.11),
			CountryCode:      		0x00,
			TotalHIDReports:        0x01,
			HIDReportType:    		DTYPE_Report,
			HIDReportLength:        sizeof(MouseReport)
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

USB_Descriptor_String_t LanguageString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(1), Type: DTYPE_String},
		
	UnicodeString:          {LANGUAGE_ID_ENG}
};

USB_Descriptor_String_t ManufacturerString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(11), Type: DTYPE_String},
		
	UnicodeString:          L"Dean Camera"
};

USB_Descriptor_String_t ProductString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(16), Type: DTYPE_String},
		
	UnicodeString:          L"MyUSB Mouse Demo"
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
		case DTYPE_HID:
			Address = DESCRIPTOR_ADDRESS(ConfigurationDescriptor.MouseHID);
			Size    = sizeof(USB_Descriptor_HID_t);
			break;
		case DTYPE_Report:
			Address = DESCRIPTOR_ADDRESS(MouseReport);
			Size    = sizeof(MouseReport);
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
