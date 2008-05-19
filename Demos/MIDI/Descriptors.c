/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Descriptors.h"

USB_Descriptor_Device_t DeviceDescriptor PROGMEM =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       0x0101,
	Class:                  0x00,
	SubClass:               0x00,
	Protocol:               0x00,
				
	Endpoint0Size:          8,
		
	VendorID:               0x03EB,
	ProductID:              0x2048,
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
			Header:                   {Size: sizeof(USB_Descriptor_Configuration_Header_t), Type: DTYPE_Configuration},

			TotalConfigurationSize:   sizeof(USB_Descriptor_Configuration_t),
			TotalInterfaces:          2,

			ConfigurationNumber:      1,
			ConfigurationStrIndex:    NO_DESCRIPTOR_STRING,
				
			ConfigAttributes:         (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
			
			MaxPowerConsumption:      USB_CONFIG_POWER_MA(100)
		},
		
	AudioControlInterface:
		{
			Header:                   {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:          0,
			AlternateSetting:         0,
			
			TotalEndpoints:           0,
				
			Class:                    0x01,
			SubClass:                 0x01,
			Protocol:                 0x00,
				
			InterfaceStrIndex:        NO_DESCRIPTOR_STRING			
		},
	
	AudioControlInterface_SPC:
		{
			Header:                   {Size: sizeof(USB_AudioInterface_AC_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_Header,
			
			ACSpecification:          0x0100,
			TotalLength:              sizeof(USB_AudioInterface_AC_t),
			
			InCollection:             1,
			InterfaceNumbers:         {1},			
		},

	AudioStreamInterface:
		{
			Header:                   {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:          1,
			AlternateSetting:         0,
			
			TotalEndpoints:           2,
				
			Class:                    0x01,
			SubClass:                 0x03,
			Protocol:                 0x00,
				
			InterfaceStrIndex:        NO_DESCRIPTOR_STRING
		},
		
	AudioStreamInterface_SPC:
		{
			Header:                   {Size: sizeof(USB_AudioInterface_AS_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_General,
			
			TerminalLink:             0x00,
			
			FrameDelay:               1,
			AudioFormat:              0x0041
		},

	MIDI_In_Jack_Emb:
		{
			Header:                   {Size: sizeof(USB_MIDI_In_Jack_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_InputJack,
			
			JackType:                 JACKTYPE_EMBEDDED,
			JackID:                   0x01,
			
			JackStrIndex:             NO_DESCRIPTOR_STRING
		},

	MIDI_In_Jack_Ext:
		{
			Header:                   {Size: sizeof(USB_MIDI_In_Jack_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_InputJack,
			
			JackType:                 JACKTYPE_EXTERNAL,
			JackID:                   0x02,
			
			JackStrIndex:             NO_DESCRIPTOR_STRING
		},
		
	MIDI_Out_Jack_Emb:
		{
			Header:                   {Size: sizeof(USB_MIDI_Out_Jack_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_OutputJack,
			
			JackType:                 JACKTYPE_EMBEDDED,
			JackID:                   0x03,

			NumberOfPins:             1,
			SourceJackID:             {0x02},
			SourcePinID:              {0x01},
			
			JackStrIndex:             NO_DESCRIPTOR_STRING
		},

	MIDI_Out_Jack_Ext:
		{
			Header:                   {Size: sizeof(USB_MIDI_Out_Jack_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_OutputJack,
			
			JackType:                 JACKTYPE_EXTERNAL,
			JackID:                   0x04,

			NumberOfPins:             1,
			SourceJackID:             {0x01},
			SourcePinID:              {0x01},
			
			JackStrIndex:             NO_DESCRIPTOR_STRING
		},

	MIDI_In_Jack_Endpoint:
		{
			Endpoint:
				{
					Header:              {Size: sizeof(USB_AudioStreamEndpoint_Std_t), Type: DTYPE_Endpoint},

					EndpointAddress:     (ENDPOINT_DESCRIPTOR_DIR_OUT | MIDI_STREAM_OUT_EPNUM),
					Attributes:          (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
					EndpointSize:        MIDI_STREAM_EPSIZE,
					PollingIntervalMS:   0
				},
			
			Refresh:                  0,
			SyncEndpointNumber:       0
		},
		
	MIDI_In_Jack_Endpoint_SPC:
		{
			Header:                   {Size: sizeof(USB_MIDI_Jack_Endpoint_t), Type: DTYPE_AudioEndpoint},
			Subtype:                  DSUBTYPE_General,

			TotalEmbeddedJacks:       0x01,
			AssociatedJackID:         {0x01}
		},

	MIDI_Out_Jack_Endpoint:
		{
			Endpoint:
				{
					Header:              {Size: sizeof(USB_AudioStreamEndpoint_Std_t), Type: DTYPE_Endpoint},

					EndpointAddress:     (ENDPOINT_DESCRIPTOR_DIR_IN | MIDI_STREAM_IN_EPNUM),
					Attributes:          (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
					EndpointSize:        MIDI_STREAM_EPSIZE,
					PollingIntervalMS:   0
				},
			
			Refresh:                  0,
			SyncEndpointNumber:       0
		},
		
	MIDI_Out_Jack_Endpoint_SPC:
		{
			Header:                   {Size: sizeof(USB_MIDI_Jack_Endpoint_t), Type: DTYPE_AudioEndpoint},
			Subtype:                  DSUBTYPE_General,

			TotalEmbeddedJacks:       0x01,
			AssociatedJackID:         {0x03}
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
	Header:                 {Size: USB_STRING_LEN(15), Type: DTYPE_String},
		
	UnicodeString:          {'M','y','U','S','B',' ','M','I','D','I',' ','D','e','m','o'}
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
	}
	
	if (DescriptorAddress != NULL)
	{
		*DescriptorAddr = DescriptorAddress;
		*Size           = DescriptorSize;

		return true;
	}
		
	return false;
}
