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
		#include "../HighLevel/StdDescriptors.h"
		
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define DESCRIPTOR_PCAST(DescriptorPtr, Type) ((Type*)DescriptorPtr)
			#define DESCRIPTOR_CAST(DescriptorPtr, Type)  (*DESCRIPTOR_PCAST(DescriptorPtr, Type))
			#define DESCRIPTOR_TYPE(DescriptorPtr)        DESCRIPTOR_CAST(DescriptorPtr, USB_Descriptor_Header_t).Type
			#define DESCRIPTOR_SIZE(DescriptorPtr)        DESCRIPTOR_CAST(DescriptorPtr, USB_Descriptor_Header_t).Size

			#define DESCRIPTOR_COMPARATOR(name)           uint8_t DCOMP_##name (void* CurrentDescriptor)

			#define USB_Host_GetNextDescriptorComp(DSize, DPos, DSearch) \
			                                              USB_Host_GetNextDescriptorComp_P(DSize, DPos, DCOMP_##DSearch)
		/* Enums: */
			enum DSEARCH_Return_ErrorCodes_t
			{
				Descriptor_Search_Found                = 0,
				Descriptor_Search_Fail                 = 1,
				Descriptor_Search_NotFound             = 2,
			};

			enum DSEARCH_Comp_Return_ErrorCodes_t
			{
				Descriptor_Search_Comp_Found           = 0,
				Descriptor_Search_Comp_Fail            = 1,
				Descriptor_Search_Comp_EndOfDescriptor = 2,
			};
	
		/* Function Prototypes: */
			uint8_t USB_Host_GetDeviceConfigDescriptor(uint16_t* const ConfigSizePtr, void* BufferPtr)
			                                           ATTR_NON_NULL_PTR_ARG(1);

		/* Inline Functions: */
			static inline void USB_Host_GetNextDescriptor(uint16_t* const BytesRem,
			                                              uint8_t** const CurrConfigLoc) 
														  ATTR_NON_NULL_PTR_ARG(1, 2);									  
			static inline void USB_Host_GetNextDescriptor(uint16_t* const BytesRem,
			                                              uint8_t** const CurrConfigLoc)
			{
				uint16_t CurrDescriptorSize = DESCRIPTOR_CAST(*CurrConfigLoc, USB_Descriptor_Header_t).Size;

				*CurrConfigLoc += CurrDescriptorSize;
				*BytesRem      -= CurrDescriptorSize;
			}


			static inline void USB_Host_GetNextDescriptorOfType(uint16_t* const BytesRem,
			                                                    uint8_t** const CurrConfigLoc,
			                                                    const uint8_t Type)
			                                                    ATTR_NON_NULL_PTR_ARG(1, 2);
			static inline void USB_Host_GetNextDescriptorOfType(uint16_t* const BytesRem,
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

			static inline void USB_Host_GetNextDescriptorOfTypeBefore(uint16_t* const BytesRem,
			                                                          uint8_t** const CurrConfigLoc,
			                                                          const uint8_t Type,
			                                                          const uint8_t BeforeType)
			                                                          ATTR_NON_NULL_PTR_ARG(1, 2);
			static inline void USB_Host_GetNextDescriptorOfTypeBefore(uint16_t* const BytesRem,
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

			static inline void USB_Host_GetNextDescriptorOfTypeAfter(uint16_t* const BytesRem,
			                                                         uint8_t** const CurrConfigLoc,
			                                                         const uint8_t Type,
																	 const uint8_t AfterType)
			                                                         ATTR_NON_NULL_PTR_ARG(1, 2);
			static inline void USB_Host_GetNextDescriptorOfTypeAfter(uint16_t* const BytesRem,
			                                                         uint8_t** const CurrConfigLoc,
			                                                         const uint8_t Type,
			                                                         const uint8_t AfterType)
			{
				USB_Host_GetNextDescriptorOfType(BytesRem, CurrConfigLoc, AfterType);
				
				if (*BytesRem)
				  USB_Host_GetNextDescriptorOfType(BytesRem, CurrConfigLoc, Type);
			}
			
	/* Private Interface - For use in library only: */
		/* Function Prototypes: */
			uint8_t USB_Host_GetNextDescriptorComp_P(uint16_t* BytesRem, uint8_t** CurrConfigLoc,
                                                     uint8_t (*SearchRoutine)(void*));

#endif
