/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "../LowLevel/USBMode.h"
#if defined(USB_CAN_BE_DEVICE)

#include "StdDescriptors.h"

bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index, const uint16_t LanguageID,
			           void** const DescriptorAddr, uint16_t* const DescriptorSize)
{
	RAISE_EVENT(USB_DeviceError, DEVICE_ERROR_GetDescriptorNotHooked);

	return false;
};

#endif
