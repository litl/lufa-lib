#include "Descriptors.h"

/* Configure device descriptor here: */
	USB_Descriptor_Device_t DeviceDescriptor PROGMEM =
	{
		Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
		USBSpecification:       0x0200,
		Class:                  0xFF,
		SubClass:               0x00,	
		Protocol:               0x00,
				
		Endpoint0Size:          ENDPOINT_CONTROLEP_SIZE,
		
		VendorID:               0x03EB,
		ProductID:              0x201C,
		ReleaseNumber:          0x1000,
		
		ManafacturerStrIndex:   0x01,
		ProductStrIndex:        0x02,
		SerialNumStrIndex:      0x03,
		
		NumberOfConfigurations: CONFIGURATIONS
	};
	
/* Configure configuration descriptor here: */
	USB_Descriptor_Configuration_t ConfigurationDescriptor PROGMEM =
	{
		Config:
			{
				Header:                 {Size: sizeof(USB_Descriptor_Configuration_Header_t), Type: DTYPE_Configuration},

				TotalConfigurationSize: (  sizeof(USB_Descriptor_Configuration_Header_t)
				                         + sizeof(USB_Descriptor_Interface_t)            ),
				TotalInterfaces:        1,
				
				ConfigurationNumber:    1,
				ConfigurationStrIndex:  NO_DESCRIPTOR_STRING,
				
				ConfigAttributes:       USB_CONFIG_ATTR_BUSPOWERED,
				
				MaxPowerConsumption:    USB_CONFIG_POWER_MA(100)
			},
		
		Interface:
			{
				Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

				InterfaceNumber:        0,
				AlternateSetting:       0,
				
				TotalEndpoints:         0,
				
				Class:                  0xFF,
				SubClass:               0x00,
				Protocol:               0x00,
				
				InterfaceStrIndex:      NO_DESCRIPTOR_STRING
			}
	};
	
/* Configure any descriptor strings here: */
	USB_Descriptor_Language_t LanguageString PROGMEM =
	{
		Header:                 {Size: sizeof(USB_Descriptor_Language_t), Type: DTYPE_String},
		
		LanguageID:             LANGUAGE_ID
	};

	USB_Descriptor_String_t ManafacturerString PROGMEM =
	{
		Header:                 {Size: USB_STRING_LEN(11), Type: DTYPE_String},
		
		UnicodeString:          {'D','E','A','N',' ','C','A','M','E','R','A'}
	};

	USB_Descriptor_String_t ProductString PROGMEM =
	{
		Header:                 {Size: USB_STRING_LEN(10), Type: DTYPE_String},
		
		UnicodeString:          {'M','Y','U','S','B',' ','D','E','M','O'}
	};

	USB_Descriptor_String_t VersionString PROGMEM =
	{
		Header:                 {Size: USB_STRING_LEN(5), Type: DTYPE_String},
		
		UnicodeString:          {'0','.','1','.','0'}
	};

/* Add handlers for the descriptor strings here: */
	bool USB_GetDescriptorString(const uint8_t Index, void** StringDescriptorAddr, uint16_t* Size)
	{
		switch (Index)
		{
			case 0x00:
				*StringDescriptorAddr = (void*)&LanguageString;
				*Size                 = sizeof(USB_Descriptor_Language_t);
				return true;		
			case 0x01:
				*StringDescriptorAddr = (void*)&ManafacturerString;
				*Size                 = USB_STRING_LEN(11);
				return true;
			case 0x02:
				*StringDescriptorAddr = (void*)&ProductString;
				*Size                 = USB_STRING_LEN(10);
				return true;
			case 0x03:
				*StringDescriptorAddr = (void*)&VersionString;
				*Size                 = USB_STRING_LEN(5);
				return true;
		}
		
		return false;
	}
