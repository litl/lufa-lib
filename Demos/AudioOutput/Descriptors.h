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

		#define DSUBTYPE_Header          0x01

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
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            AudioInterface;
			USB_AudioInterface_AC_t               AudioInterfaceAC;
		} USB_Descriptor_Configuration_t;
		
	/* Function Prototypes: */
		bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index,
							   void** const DescriptorAddr, uint16_t* const Size)
							   ATTR_WARN_UNUSED_RESULT;

#endif
