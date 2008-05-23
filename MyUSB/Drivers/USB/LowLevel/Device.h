/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __USBDEVICE_H__
#define __USBDEVICE_H__

	/* Includes: */
		#include <avr/pgmspace.h>
		#include <avr/eeprom.h>

		#include "../../../Common/Common.h"	
		#include "../HighLevel/StdDescriptors.h"
		#include "Endpoint.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#if defined(USB_FULL_CONTROLLER) || defined(__DOXYGEN__)
				#define USB_DEVICE_OPT_LOWSPEED            (1 << 0)
			#endif
			
			#define USB_DEVICE_OPT_FULLSPEED               (0 << 0)
				
			#define USB_Device_SendRemoteWakeup()   MACROS{ UDCON |= (1 << RMWKUP); }MACROE
			#define USB_Device_IsRemoteWakeupSent()       ((UDCON &  (1 << RMWKUP)) ? false : true)
			#define USB_Device_IsUSBSuspended()           ((UDINT &  (1 << SUSPI)) ? true : false)

			#define USB_Detach()                    MACROS{ UDCON  |=  (1 << DETACH);  }MACROE
			#define USB_Attach()                    MACROS{ UDCON  &= ~(1 << DETACH);  }MACROE

		/* Enums: */
			enum USB_Device_ErrorCodes_t
			{
				DEVICE_ERROR_GetDescriptorNotHooked        = 0,
			};

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */		
			#define USB_Device_SetLowSpeed()        MACROS{ UDCON |=  (1 << LSM);   }MACROE
			#define USB_Device_SetHighSpeed()       MACROS{ UDCON &= ~(1 << LSM);   }MACROE
	#endif
	
#endif
