/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <MyUSB/Drivers/USB/USB.h>

	/* Macros: */
		#define DTYPE_AudioInterface        0x24
		#define DTYPE_AudioEndpoint         0x25

		#define DSUBTYPE_Header             0x01
		#define DSUBTYPE_General            0x01
		#define DSUBTYPE_InputTerminal      0x02
		#define DSUBTYPE_OutputTerminal     0x03
		
		#define CHANNEL_LEFT_FRONT          (1 << 0)
		#define CHANNEL_RIGHT_FRONT         (1 << 1)
		#define CHANNEL_CENTER_FRONT        (1 << 2)
		#define CHANNEL_LOW_FREQ_ENHANCE    (1 << 3)
		#define CHANNEL_LEFT_SURROUND       (1 << 4)
		#define CHANNEL_RIGHT_SURROUND      (1 << 5)
		#define CHANNEL_LEFT_OF_CENTER      (1 << 6)
		#define CHANNEL_RIGHT_OF_CENTER     (1 << 7)
		#define CHANNEL_SURROUND            (1 << 8)
		#define CHANNEL_SIDE_LEFT           (1 << 9)
		#define CHANNEL_SIDE_RIGHT          (1 << 10)
		#define CHANNEL_TOP                 (1 << 11)

		#define AUDIO_STREAM_EPNUM          0x01
		#define AUDIO_STREAM_EPSIZE         64

	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Header_t Header;
			uint8_t                 Subtype;

			uint16_t                ACSpecification;
			uint16_t                TotalLength;
			
			uint8_t                 InCollection;
			uint8_t                 InterfaceNumbers[1];			
		} USB_AudioInterface_AC_t;
		
		typedef struct
		{
			USB_Descriptor_Header_t Header;
			uint8_t                 Subtype;
		
			uint8_t                 TerminalID;
			uint16_t                TerminalType;
			uint8_t                 AssociatedOutputTerminal;
			
			uint8_t                 TotalChannels;
			uint16_t                ChannelConfig;
			
			uint8_t                 ChannelStrIndex;
			uint8_t                 TerminalStrIndex;
		} USB_AudioInputTerminal_t;

		typedef struct
		{
			USB_Descriptor_Header_t Header;
			uint8_t                 Subtype;
		
			uint8_t                 TerminalID;
			uint16_t                TerminalType;
			uint8_t                 AssociatedInputTerminal;
			
			uint8_t                 SourceID;
			
			uint8_t                 TerminalStrIndex;
		} USB_AudioOutputTerminal_t;
		
		typedef struct
		{
			USB_Descriptor_Header_t   Header;
			uint8_t                   Subtype;
			
			uint8_t                   TerminalLink;
			
			uint8_t                   FrameDelay;
			uint16_t                  AudioFormat;
		} USB_AudioInterface_AS_t;
		
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
			
			uint8_t                   Attributes;

			uint8_t                   LockDelayUnits;
			uint8_t                   LockDelay;
		} USB_AudioStreamEndpoint_Spc_t;	

		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            AudioControlInterface;
			USB_AudioInterface_AC_t               AudioControlInterface_SPC;
			USB_AudioInputTerminal_t              InputTerminal;
			USB_AudioOutputTerminal_t             OutputTerminal;
			USB_Descriptor_Interface_t            AudioStreamInterface_Alt0;
			USB_Descriptor_Interface_t            AudioStreamInterface_Alt1;
			USB_AudioInterface_AS_t               AudioStreamInterface_SPC;
			USB_AudioStreamEndpoint_Std_t         AudioEndpoint;
			USB_AudioStreamEndpoint_Spc_t         AudioEndpoint_SPC;
		} USB_Descriptor_Configuration_t;
		
	/* Function Prototypes: */
		bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index,
							   void** const DescriptorAddr, uint16_t* const Size)
							   ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3, 4);

#endif
