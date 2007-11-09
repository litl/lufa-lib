/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef USBDEVICE_H
#define USBDEVICE_H

	/* Includes: */
		#include "../../../Common/Common.h"
		#include "../../../Common/FunctionAttributes.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define USB_DEV_LOWSPEED                (1 << 0)
			#define USB_DEV_HIGHSPEED               (0 << 0)

		/* Enums: */
			enum USB_Device_ErrorCodes
			{
				DEVICE_ERROR_GetDescriptorNotHooked        = 0,
				DEVICE_ERROR_ControlEndpointCreationFailed = 1,
			};

		/* Inline Functions: */
			static inline uint8_t USB_Device_Read_Byte(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t USB_Device_Read_Byte(void)
			{
				return UEDATX;
			}

			static inline void USB_Device_Write_Byte(const uint8_t Byte)
			{
				UEDATX = Byte;
			}

			static inline void USB_Device_Ignore_Byte(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UEDATX;
			}
			
			static inline uint16_t USB_Device_Read_Word(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint16_t USB_Device_Read_Word(void)
			{
				uint16_t Data;
				
				Data  = UEDATX;
				Data |= (((uint16_t)UEDATX) << 8);
			
				return Data;
			}

			static inline void USB_Device_Write_Word(const uint16_t Byte)
			{
				UEDATX = (Byte & 0xFF);
				UEDATX = (Byte >> 8);
			}
			
			static inline void USB_Device_Ignore_Word(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UEDATX;
				Dummy = UEDATX;
			}

	/* Private Interface - For use in library only: */
		/* Macros: */		
			#define USB_DEV_SetLowSpeed()   MACROS{ UDCON |=  (1 << LSM); }MACROE
			#define USB_DEV_SetHighSpeed()  MACROS{ UDCON &= ~(1 << LSM); }MACROE
		
#endif
