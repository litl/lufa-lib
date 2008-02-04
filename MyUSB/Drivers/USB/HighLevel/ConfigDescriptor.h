/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __CONFIG_DESCRIPTOR_H__
#define __CONFIG_DESCRIPTOR_H__

	/* Includes: */
		#include <avr/io.h>
		
		#include "../../../Common/Common.h"
		#include "../LowLevel/HostChapter9.h"
		#include "StdDescriptors.h"
		
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define DESCRIPTOR_PCAST(DescriptorPtr, Type) ((Type*)DescriptorPtr)
			#define DESCRIPTOR_CAST(DescriptorPtr, Type)  (*DESCRIPTOR_PCAST(DescriptorPtr, Type))
			#define DESCRIPTOR_TYPE(DescriptorPtr)        DESCRIPTOR_CAST(DescriptorPtr, USB_Descriptor_Header_t).Type
			#define DESCRIPTOR_SIZE(DescriptorPtr)        DESCRIPTOR_CAST(DescriptorPtr, USB_Descriptor_Header_t).Size
	
		/* Inline Functions: */
			static inline uint8_t AVR_HOST_GetDeviceConfigDescriptor(uint16_t* const ConfigSizePtr,
																	 uint8_t* BufferPtr)
																	 ATTR_NON_NULL_PTR_ARG(1);

			static inline uint8_t AVR_HOST_GetDeviceConfigDescriptor(uint16_t* const ConfigSizePtr, uint8_t* BufferPtr)
			{
				uint8_t ErrorCode;

				USB_HostRequest = (USB_Host_Request_Header_t)
					{
						RequestType: (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE),
						RequestData: REQ_GetDescriptor,
						Value:       (DTYPE_Configuration << 8),
						Index:       0,
						DataLength:  0,
					};
				
				if (BufferPtr == NULL)
				{
					BufferPtr = alloca(sizeof(USB_Descriptor_Configuration_Header_t));

					USB_HostRequest.DataLength = sizeof(USB_Descriptor_Configuration_Header_t);					
					ErrorCode      = USB_Host_SendControlRequest(BufferPtr);

					*ConfigSizePtr = ((USB_Descriptor_Configuration_Header_t*)BufferPtr)->TotalConfigurationSize;
				}
				else
				{
					USB_HostRequest.DataLength = *ConfigSizePtr;
					
					ErrorCode      = USB_Host_SendControlRequest(BufferPtr);				
				}

				return ErrorCode;
			}

			static inline void AVR_HOST_GetNextDescriptor(uint16_t* const BytesRem, uint8_t** const CurrConfigLoc) 
														  ATTR_NON_NULL_PTR_ARG(1, 2);									  
			static inline void AVR_HOST_GetNextDescriptor(uint16_t* const BytesRem, uint8_t** const CurrConfigLoc)
			{
				uint16_t CurrDescriptorSize = ((USB_Descriptor_Header_t*)*CurrConfigLoc)->Size;

				*CurrConfigLoc += CurrDescriptorSize;
				*BytesRem      -= CurrDescriptorSize;
			}
#endif
