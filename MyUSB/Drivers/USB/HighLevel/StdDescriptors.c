/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
#include "StdDescriptors.h"

bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index,
			           void** const DescriptorAddr, uint16_t* const Size)
{
	RAISE_EVENT(USB_DeviceError, DEVICE_ERROR_GetDescriptorNotHooked);

	return false;
};
#endif
