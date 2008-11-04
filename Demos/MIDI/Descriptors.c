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

USB_Descriptor_Device_t DeviceDescriptor PROGMEM =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       VERSION_BCD(01.10),
	Class:                  0x00,
	SubClass:               0x00,
	Protocol:               0x00,
				
	Endpoint0Size:          8,
		
	VendorID:               0x03EB,
	ProductID:              0x2048,
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
			Header:                   {Size: sizeof(USB_AudioInterface_MIDI_AS_t), Type: DTYPE_AudioInterface},
			Subtype:                  DSUBTYPE_General,

			AudioSpecification:       VERSION_BCD(01.00),
			
			TotalLength:              (sizeof(USB_Descriptor_Configuration_t) - offsetof(USB_Descriptor_Configuration_t, AudioStreamInterface_SPC))
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
	Header:                 {Size: USB_STRING_LEN(15), Type: DTYPE_String},
		
	UnicodeString:          L"MyUSB MIDI Demo"
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
