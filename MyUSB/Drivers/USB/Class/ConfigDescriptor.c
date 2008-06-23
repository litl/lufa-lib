/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "ConfigDescriptor.h"

uint8_t USB_Host_GetDeviceConfigDescriptor(uint16_t* const ConfigSizePtr, void* BufferPtr)
{
	uint8_t ErrorCode;

	USB_HostRequest = (USB_Host_Request_Header_t)
		{
			bmRequestType: (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE),
			bRequest:      REQ_GetDescriptor,
			wValue:        (DTYPE_Configuration << 8),
			wIndex:        0,
			wLength:       sizeof(USB_Descriptor_Configuration_Header_t),
		};
	
	if (BufferPtr == NULL)
	{
		BufferPtr      = alloca(sizeof(USB_Descriptor_Configuration_Header_t));

		ErrorCode      = USB_Host_SendControlRequest(BufferPtr);

		#if defined(USE_NONSTANDARD_DESCRIPTOR_NAMES)
		*ConfigSizePtr = DESCRIPTOR_CAST(BufferPtr, USB_Descriptor_Configuration_Header_t).TotalConfigurationSize;
		#else
		*ConfigSizePtr = DESCRIPTOR_CAST(BufferPtr, USB_Descriptor_Configuration_Header_t).wTotalLength;		
		#endif
	}
	else
	{
		USB_HostRequest.wLength = *ConfigSizePtr;
		
		ErrorCode      = USB_Host_SendControlRequest(BufferPtr);				
	}

	return ErrorCode;
}

void USB_Host_GetNextDescriptorOfType(uint16_t* const BytesRem,
                                      uint8_t** const CurrConfigLoc,
                                      const uint8_t Type)
{
	while (*BytesRem)
	{
		USB_Host_GetNextDescriptor(BytesRem, CurrConfigLoc);	  

		if (DESCRIPTOR_TYPE(*CurrConfigLoc) == Type)
		  return;
	}
}

void USB_Host_GetNextDescriptorOfTypeBefore(uint16_t* const BytesRem,
                                            uint8_t** const CurrConfigLoc,
                                            const uint8_t Type,
                                            const uint8_t BeforeType)
{
	while (*BytesRem)
	{
		USB_Host_GetNextDescriptor(BytesRem, CurrConfigLoc);

		if (DESCRIPTOR_TYPE(*CurrConfigLoc) == Type)
		{
			return;
		}
		else if (DESCRIPTOR_TYPE(*CurrConfigLoc) == BeforeType)
		{
			*BytesRem = 0;
			return;
		}
	}
}

void USB_Host_GetNextDescriptorOfTypeAfter(uint16_t* const BytesRem,
                                           uint8_t** const CurrConfigLoc,
                                           const uint8_t Type,
                                           const uint8_t AfterType)
{
	USB_Host_GetNextDescriptorOfType(BytesRem, CurrConfigLoc, AfterType);
	
	if (*BytesRem)
	  USB_Host_GetNextDescriptorOfType(BytesRem, CurrConfigLoc, Type);
}
			
uint8_t USB_Host_GetNextDescriptorComp_P(uint16_t* BytesRem, uint8_t** CurrConfigLoc,
                                         uint8_t (*SearchRoutine)(void*))
{
	uint8_t ErrorCode;
		
	while (*BytesRem)
	{
		USB_Host_GetNextDescriptor(BytesRem, CurrConfigLoc);

		ErrorCode = SearchRoutine(*CurrConfigLoc);
		
		if (ErrorCode == Descriptor_Search_Fail)
		  return Descriptor_Search_Comp_Fail;
		else if (ErrorCode == Descriptor_Search_Found)
		  return Descriptor_Search_Comp_Found;
	}
	
	return Descriptor_Search_Comp_EndOfDescriptor;
}
