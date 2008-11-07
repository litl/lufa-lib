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
		
	USBSpecification:       VERSION_BCD(02.00),
	Class:                  0x00,
	SubClass:               0x00,
	Protocol:               0x00,
				
	Endpoint0Size:          8,
		
	VendorID:               0x03EB,
	ProductID:              0x2047,
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
			
			ACSpecification:          VERSION_BCD(01.00),
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
			TerminalType:             TERMINAL_IN_MIC,
			AssociatedOutputTerminal: 0x00,
			
			TotalChannels:            1,
			ChannelConfig:            0,
			
			ChannelStrIndex:          NO_DESCRIPTOR_STRING,
			TerminalStrIndex:         NO_DESCRIPTOR_STRING
		},

	OutputTerminal:
		{
			Header:                   {Size: sizeof(USB_AudioOutputTerminal_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_OutputTerminal,
		
			TerminalID:               0x02,
			TerminalType:             TERMINAL_STREAMING,
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
			
			TerminalLink:             0x02,
			
			FrameDelay:               1,
			AudioFormat:              0x0001
		},
		
	AudioFormat:
		{
			Header:                   {Size: sizeof(USB_AudioFormat_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_Format,

			FormatType:               0x01,
			Channels:                 0x01,
			
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

					EndpointAddress:     (ENDPOINT_DESCRIPTOR_DIR_IN | AUDIO_STREAM_EPNUM),
					Attributes:          (EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_SYNC | ENDPOINT_USAGE_DATA),
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
	Header:                 {Size: USB_STRING_LEN(19), Type: DTYPE_String},
		
	UnicodeString:          L"MyUSB Audio In Demo"
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
