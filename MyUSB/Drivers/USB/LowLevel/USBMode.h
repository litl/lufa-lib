/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __USB_MODE_H__
#define __USB_MODE_H__

	/* Private Interface - For use in library only: */
		/* Macros: */
			#if ((defined(__AVR_AT90USB1286__) || (defined(__AVR_AT90USB646__))) && !(defined(USB_DEVICE_ONLY)))
				#define USB_DEVICE_ONLY
			#endif

			#if (!defined(USB_DEVICE_ONLY) && !defined(USB_HOST_ONLY))
				#define USB_CAN_BE_BOTH
				#define USB_CAN_BE_HOST
				#define USB_CAN_BE_DEVICE
			#elif defined(USB_HOST_ONLY)
				#define USB_CAN_BE_HOST
			#elif defined(USB_DEVICE_ONLY)
				#define USB_CAN_BE_DEVICE
			#endif

#endif
