/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  USB Device mode related macros and enums. This module contains macros and enums which are used when
 *  the USB controller is initialized in device mode.
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
				/** Mask for the Options parameter of the USB_Init() function. This indicates that the
				 *  USB interface should be initialized in low speed (1.5Mb/s) mode.
				 *
				 *  \note Low Speed mode is not available on all USB AVR models.
				 *
				 *  \note Restrictions apply on the number, size and type of endpoints which can be used
				 *        when running in low speed mode -- refer to the USB 2.0 standard.
				 */
				#define USB_DEVICE_OPT_LOWSPEED            (1 << 0)
			#endif
			
			/** Mask for the Options parameter of the USB_Init() function. This indicates that the
			 *  USB interface should be initialized in full speed (12Mb/s) mode.
			 */
			#define USB_DEVICE_OPT_FULLSPEED               (0 << 0)
			
			/** Sends a Remote Wakeup request to the host. This signals to the host that the device should
			 *  be taken out of suspended mode, and communications should resume.
			 *
			 *  Typically, this is implemented so that HID devices (mice, keyboards, etc.) can wake up the
			 *  host computer when the host has suspended all USB devices to enter a low power state.
			 *
			 *  \note This macro should only be used if the device has indicated to the host that it
			 *        supports the Remote Wakeup feature in the device descriptors.
			 *
			 *  \see StdDescriptors.h for more information on the RMWAKEUP feature and device descriptors.
			 */
			#define USB_Device_SendRemoteWakeup()   MACROS{ UDCON |= (1 << RMWKUP); }MACROE

			/** Indicates if a Remote Wakeup request is being sent to the host. This returns true if a
			 *  remote wakeup is currently being sent, false otherwise.
			 *
			 *  This can be used in conjunction with the USB_Device_IsUSBSuspended() macro to determine if
			 *  a sent RMWAKEUP request was accepted or rejected by the host.
			 *
			 *  \note This macro should only be used if the device has indicated to the host that it
			 *        supports the Remote Wakeup feature in the device descriptors.
			 *
			 *  \see StdDescriptors.h for more information on the RMWAKEUP feature and device descriptors.
			 */
			#define USB_Device_IsRemoteWakeupSent()       ((UDCON &  (1 << RMWKUP)) ? false : true)

			/** Indicates if the device is currently suspended by the host. While suspended, the device is
			 *  to enter a low power state until resumed by the host. While suspended no USB traffic to or
			 *  from the device can ocurr (except for Remote Wakeup requests).
			 *
			 *  This macro returns true if the USB communications have been suspended by the host, false
			 *  otherwise.
			 */
			#define USB_Device_IsUSBSuspended()           ((UDINT &  (1 << SUSPI)) ? true : false)

			/** Detaches the device from the USB bus. This has the effect of removing the device from any
			 *  host if, ceasing USB communications. If no host is present, this prevents any host from
			 *  enumerating the device once attached until USB_Attach() is called.
			 */
			#define USB_Detach()                    MACROS{ UDCON  |=  (1 << DETACH);  }MACROE

			/** Attaches the device to the USB bus. This announces the device's presence to any attached
			 *  USB host, starting the enumeration process. If no host is present, attaching the device
			 *  will allow for enumeration once a host is connected to the device.
			 */
			#define USB_Attach()                    MACROS{ UDCON  &= ~(1 << DETACH);  }MACROE

		/* Enums: */
			/** Enum for the ErrorCode parameter of the USB_DeviceError event.
			 *
			 *  \see Events.h for more information on this event.
			 */
			enum USB_Device_ErrorCodes_t
			{
				DEVICE_ERROR_GetDescriptorNotHooked        = 0, /**< Indicates that the GetDescriptor() method
				                                                 *   has not been hooked by the user application.
				                                                 *
				                                                 *   \see StdDescriptors.h for more information on
				                                                 *        the GetDescriptor() method.
				                                                 */
			};

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */		
			#define USB_Device_SetLowSpeed()        MACROS{ UDCON |=  (1 << LSM);   }MACROE
			#define USB_Device_SetHighSpeed()       MACROS{ UDCON &= ~(1 << LSM);   }MACROE
	#endif
	
#endif
