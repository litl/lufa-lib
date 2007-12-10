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
		#define DTYPE_AudioInterface     0x24
		#define DTYPE_AudioEndpoint      0x25

		#define DSUBTYPE_Header          0x01
		#define DSUBTYPE_General         0x01

	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Header_t Header;
			uint16_t                Subtype;

			uint16_t                ACSpecification;
			uint16_t                TotalLength;
			
			uint8_t                 InCollection;
			uint8_t                 InterfaceNumbers[1];			
		} USB_AudioInterface_AC_t;
		
		typedef struct
		{
			USB_Descriptor_Header_t Header;
			uint16_t                Subtype;
		
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
			uint16_t                Subtype;
		
			uint8_t                 TerminalID;
			uint16_t                TerminalType;
			uint8_t                 AssociatedInputTerminal;
			
			uint8_t                 SourceID;
			
			uint8_t                 TerminalStrIndex;
		} USB_AudioOutputTerminal_t;
		
		typedef struct
		{
			USB_Descriptor_Endpoint_t Endpoint;

			uint8_t                   SyncEndpointNumber;
		} USB_AudioStreamEndpoint_Std_t;
					
		typedef struct
		{
			USB_Descriptor_Header_t   Header;
			uint16_t                  Subtype;
			
			uint8_t                   Attributes;

			uint8_t                   LockDelayUnits;
			uint8_t                   LockDelay;
		} USB_AudioStreamEndpoint_Spc_t;	

		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            AudioInterface;
			USB_AudioInterface_AC_t               AudioControlInterface;
			USB_AudioInputTerminal_t              InputTerminal;
			USB_AudioInputTerminal_t              OutputTerminal;
			USB_AudioStreamEndpoint_Std_t         AudioEndpoint;
			USB_AudioStreamEndpoint_Spc_t         AudioEndpointSpecific;
		} USB_Descriptor_Configuration_t;
		
	/* Function Prototypes: */
		bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index,
							   void** const DescriptorAddr, uint16_t* const Size)
							   ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3, 4);

#endif
