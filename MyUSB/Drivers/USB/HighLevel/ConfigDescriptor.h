/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef _CONFIG_DESCRIPTOR_API_H_
#define _CONFIG_DESCRIPTOR_API_H_

	/* Includes: */
		#include <avr/io.h>
		
		#include "../../../Common/FunctionAttributes.h"
		#include "../LowLevel/LowLevel.h"

	/* Function Prototypes: */
		uint8_t AVR_HOST_GetDeviceConfigDescriptorSize(uint16_t* ConfigSizePtr)
		        ATTR_NON_NULL_PTR_ARG(1);
		uint8_t AVR_HOST_GetDeviceConfigDescriptor(uint16_t BufferSize, uint8_t* BufferPtr)
		        ATTR_NON_NULL_PTR_ARG(2);
		void    AVR_HOST_GetNextDescriptor(uint16_t* BytesRem, uint8_t** CurrConfigLoc)
		        ATTR_NON_NULL_PTR_ARG(1, 2);

#endif
