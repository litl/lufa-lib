/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __USBDEVICE_H__
#define __USBDEVICE_H__

	/* Includes: */
		#include "../../../Common/Common.h"
		#include "../../../Common/FunctionAttributes.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define USB_DEV_OPT_LOWSPEED            (1 << 0)
			#define USB_DEV_OPT_HIGHSPEED           (0 << 0)

		/* Enums: */
			enum USB_Device_ErrorCodes
			{
				DEVICE_ERROR_GetDescriptorNotHooked        = 0,
				DEVICE_ERROR_ControlEndpointCreationFailed = 1,
			};

	/* Private Interface - For use in library only: */
		/* Macros: */		
			#define USB_DEV_SetLowSpeed()   MACROS{ UDCON |=  (1 << LSM); }MACROE
			#define USB_DEV_SetHighSpeed()  MACROS{ UDCON &= ~(1 << LSM); }MACROE
		
#endif
