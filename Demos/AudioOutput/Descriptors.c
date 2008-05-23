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
	ProductID:              0x2046,
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
			TotalLength:              (sizeof(USB_AudioInterface_AC_t) +
			                           sizeof(USB_AudioInputTerminal_t) +
									   sizeof(USB_AudioOutputTerminal_t)),
			
			InCollection:             1,
			InterfaceNumbers:         {1},			
		},

	InputTerminal:
		{
			Header:                   {Size: sizeof(USB_AudioInputTerminal_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_InputTerminal,
		
			TerminalID:               0x01,
			TerminalType:             TERMINAL_STREAMING,
			AssociatedOutputTerminal: 0x00,
			
			TotalChannels:            2,
			ChannelConfig:            (CHANNEL_LEFT_FRONT | CHANNEL_RIGHT_FRONT),
			
			ChannelStrIndex:          NO_DESCRIPTOR_STRING,
			TerminalStrIndex:         NO_DESCRIPTOR_STRING
		},

	OutputTerminal:
		{
			Header:                   {Size: sizeof(USB_AudioOutputTerminal_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_OutputTerminal,
		
			TerminalID:               0x02,
			TerminalType:             TERMINAL_OUT_SPEAKER,
			AssociatedInputTerminal:  0x00,
			
			SourceID:                 0x01,
			
			TerminalStrIndex:         NO_DESCRIPTOR_STRING			
		},

	AudioStreamInterface_Alt0:
		{
			Header:                   {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:          1,
			AlternateSetting:         0,
			
			TotalEndpoints:           0,
				
			Class:                    0x01,
			SubClass:                 0x02,
			Protocol:                 0x00,
				
			InterfaceStrIndex:        NO_DESCRIPTOR_STRING
		},

	AudioStreamInterface_Alt1:
		{
			Header:                   {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:          1,
			AlternateSetting:         1,
			
			TotalEndpoints:           1,
				
			Class:                    0x01,
			SubClass:                 0x02,
			Protocol:                 0x00,
				
			InterfaceStrIndex:        NO_DESCRIPTOR_STRING
		},
		
	AudioStreamInterface_SPC:
		{
			Header:                   {Size: sizeof(USB_AudioInterface_AS_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_General,
			
			TerminalLink:             0x01,
			
			FrameDelay:               1,
			AudioFormat:              0x0001
		},
		
	AudioFormat:
		{
			Header:                   {Size: sizeof(USB_AudioFormat_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_Format,

			FormatType:               0x01,
			Channels:                 0x02,
			
			SubFrameSize:             0x02,
			BitResolution:            16,
			SampleFrequencyType:      (sizeof(ConfigurationDescriptor.AudioFormat.SampleFrequencies) / sizeof(AudioSampleFreq_t)),
		
			SampleFrequencies:        {SAMPLE_FREQ(AUDIO_SAMPLE_FREQUENCY)}
		},
	
	AudioEndpoint:
		{
			Endpoint:
				{
					Header:              {Size: sizeof(USB_AudioStreamEndpoint_Std_t), Type: DTYPE_Endpoint},

					EndpointAddress:     (ENDPOINT_DESCRIPTOR_DIR_OUT | AUDIO_STREAM_EPNUM),
					Attributes:          (EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_ASYNC | ENDPOINT_USAGE_DATA),
					EndpointSize:        AUDIO_STREAM_EPSIZE,
					PollingIntervalMS:   1
				},
			
			Refresh:                  0,
			SyncEndpointNumber:       0
		},
		
	AudioEndpoint_SPC:
		{
			Header:                   {Size: sizeof(USB_AudioStreamEndpoint_Spc_t), Type: DTYPE_AudioEndpoint},
			Subtype:                  DSUBTYPE_General,
			
			Attributes:               0x00,
			
			LockDelayUnits:           0x00,
			LockDelay:                0x0000
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
		
	UnicodeString:          {'D','e','a','n',' ','C','a','m','e','r','a'}
};

USB_Descriptor_String_t ProductString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(20), Type: DTYPE_String},
		
	UnicodeString:          {'M','y','U','S','B',' ','A','u','d','i','o',' ','O','u','t',' ','D','e','m','o'}
};

USB_Descriptor_String_t SerialNumberString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(12), Type: DTYPE_String},
		
	UnicodeString:          {'0','0','0','0','0','0','0','0','0','0','0','0'}
};

bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index, const uint16_t LanguageID,
                       void** const DescriptorAddr, uint16_t* const DescriptorSize)
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
	}
	
	if (Address != NULL)
	{
		*DescriptorAddr = Address;
		*DescriptorSize = Size;

		return true;
	}
		
	return false;
}
