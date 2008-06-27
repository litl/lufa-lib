/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <MyUSB/Drivers/USB/USB.h>

		#include <avr/pgmspace.h>

	/* Macros: */
		#define DTYPE_AudioInterface        0x24
		#define DTYPE_AudioEndpoint         0x25

		#define DSUBTYPE_Header             0x01
		#define DSUBTYPE_InputJack          0x02
		#define DSUBTYPE_OutputJack         0x03

		#define DSUBTYPE_General            0x01

		#define JACKTYPE_EMBEDDED           0x01
		#define JACKTYPE_EXTERNAL           0x02
		
		#define MIDI_STREAM_OUT_EPNUM       1
		#define MIDI_STREAM_IN_EPNUM        2
		#define MIDI_STREAM_EPSIZE          64
		
	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Header_t   Header;
			uint8_t                   Subtype;

			uint16_t                  ACSpecification;
			uint16_t                  TotalLength;
			
			uint8_t                   InCollection;
			uint8_t                   InterfaceNumbers[1];			
		} USB_AudioInterface_AC_t;
		
		typedef struct
		{
			USB_Descriptor_Header_t   Header;
			uint8_t                   Subtype;
			
			uint16_t                  AudioSpecification;
			uint16_t                  TotalLength;
		} USB_AudioInterface_MIDI_AS_t;
		
		typedef struct
		{
			USB_Descriptor_Endpoint_t Endpoint;

			uint8_t                   Refresh;
			uint8_t                   SyncEndpointNumber;
		} USB_AudioStreamEndpoint_Std_t;

		typedef struct
		{
			USB_Descriptor_Header_t   Header;
			uint8_t                   Subtype;

			uint8_t                   JackType;
			uint8_t                   JackID;
			
			uint8_t                   JackStrIndex;
		} USB_MIDI_In_Jack_t;

		typedef struct
		{
			USB_Descriptor_Header_t   Header;
			uint8_t                   Subtype;

			uint8_t                   JackType;
			uint8_t                   JackID;
			
			uint8_t                   NumberOfPins;
			uint8_t                   SourceJackID[1];
			uint8_t                   SourcePinID[1];
			
			uint8_t                   JackStrIndex;
		} USB_MIDI_Out_Jack_t;
		
		typedef struct
		{
			USB_Descriptor_Header_t   Header;
			uint8_t                   Subtype;

			uint8_t                   TotalEmbeddedJacks;
			uint8_t                   AssociatedJackID[1];
		} USB_MIDI_Jack_Endpoint_t;

		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            AudioControlInterface;
			USB_AudioInterface_AC_t               AudioControlInterface_SPC;
			USB_Descriptor_Interface_t            AudioStreamInterface;
			USB_AudioInterface_MIDI_AS_t          AudioStreamInterface_SPC;
			USB_MIDI_In_Jack_t                    MIDI_In_Jack_Emb;
			USB_MIDI_In_Jack_t                    MIDI_In_Jack_Ext;
			USB_MIDI_Out_Jack_t                   MIDI_Out_Jack_Emb;
			USB_MIDI_Out_Jack_t                   MIDI_Out_Jack_Ext;
			USB_AudioStreamEndpoint_Std_t         MIDI_In_Jack_Endpoint;
			USB_MIDI_Jack_Endpoint_t              MIDI_In_Jack_Endpoint_SPC;
			USB_AudioStreamEndpoint_Std_t         MIDI_Out_Jack_Endpoint;
			USB_MIDI_Jack_Endpoint_t              MIDI_Out_Jack_Endpoint_SPC;
		} USB_Descriptor_Configuration_t;
		
	/* Function Prototypes: */
		bool USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex,
		                       void** const DescriptorAddress, uint16_t* const DescriptorSize)
		                       ATTR_WARN_UNUSED_RESULT ATTR_WEAK ATTR_NON_NULL_PTR_ARG(3, 4);

#endif
