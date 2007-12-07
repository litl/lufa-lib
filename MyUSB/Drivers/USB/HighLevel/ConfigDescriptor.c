/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
#include "ConfigDescriptor.h"

uint8_t AVR_HOST_GetDeviceConfigDescriptorSize(uint16_t* ConfigSizePtr)
{
	uint8_t ErrorCode;
	uint8_t Buffer[sizeof(USB_Descriptor_Configuration_Header_t)];

	USB_HostRequest = (USB_Host_Request_Header_t)
		{
			RequestType: (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE),
			RequestData: REQ_GetDescriptor,
			Value:       (DTYPE_Configuration << 8),
			Index:       0,
			Length:      sizeof(USB_Descriptor_Configuration_Header_t),
		};
		
	ErrorCode = USB_Host_SendControlRequest(Buffer);
	  
	*ConfigSizePtr = ((USB_Descriptor_Configuration_Header_t*)&Buffer)->TotalConfigurationSize;

	return ErrorCode;
}

uint8_t AVR_HOST_GetDeviceConfigDescriptor(uint16_t BufferSize, uint8_t* BufferPtr)
{
	uint8_t ErrorCode;

	USB_HostRequest = (USB_Host_Request_Header_t)
		{
			RequestType: (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE),
			RequestData: REQ_GetDescriptor,
			Value:       (DTYPE_Configuration << 8),
			Index:       0,
			Length:      BufferSize,
		};

	ErrorCode = USB_Host_SendControlRequest(BufferPtr);

	return ErrorCode;
}

void AVR_HOST_GetNextDescriptor(uint16_t* BytesRem, uint8_t** CurrConfigLoc)
{
	uint16_t CurrDescriptorSize = ((USB_Descriptor_Header_t*)*CurrConfigLoc)->Size;

	*CurrConfigLoc += CurrDescriptorSize;
	*BytesRem      -= CurrDescriptorSize;
}
#endif

