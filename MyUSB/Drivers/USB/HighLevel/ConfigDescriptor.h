/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __CONFIG_DESCRIPTOR_API_H__
#define __CONFIG_DESCRIPTOR_API_H__

	/* Includes: */
		#include <avr/io.h>
		
		#include "../../../Common/FunctionAttributes.h"
		#include "../LowLevel/HostChapter9.h"
		#include "StdDescriptors.h"
		
	/* Public Interface - May be used in end-application: */
		/* Function Prototypes: */
			uint8_t AVR_HOST_GetDeviceConfigDescriptorSize(uint16_t* ConfigSizePtr)
					ATTR_NON_NULL_PTR_ARG(1);
			uint8_t AVR_HOST_GetDeviceConfigDescriptor(uint16_t BufferSize, uint8_t* BufferPtr)
					ATTR_NON_NULL_PTR_ARG(2);

		/* Inline Functions */
			static inline void AVR_HOST_GetNextDescriptor(uint16_t* BytesRem, uint8_t** CurrConfigLoc) 
														  ATTR_NON_NULL_PTR_ARG(1, 2);									  
			static inline void AVR_HOST_GetNextDescriptor(uint16_t* BytesRem, uint8_t** CurrConfigLoc)
			{
				uint16_t CurrDescriptorSize = ((USB_Descriptor_Header_t*)*CurrConfigLoc)->Size;

				*CurrConfigLoc += CurrDescriptorSize;
				*BytesRem      -= CurrDescriptorSize;
			}
#endif
