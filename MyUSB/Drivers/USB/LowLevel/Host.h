/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef HOST_H
#define HOST_H

	/* Includes: */
		#include "../../../Common/Common.h"

	/* Private Macros */		
		#define USB_HOST_HostModeOn()         MACROS{ USBCON |= (1 << HOST);     }MACROE
		#define USB_HOST_HostModeOff()        MACROS{ USBCON &= ~(1 << HOST);    }MACROE

		#define USB_HOST_EnableVBUSControl()  MACROS{ OTGCON |=  (1 << VBUSHWC); DDRE |= (1 << 7); }MACROE
		#define USB_HOST_DisableVBUSControl() MACROS{ OTGCON &= ~(1 << VBUSHWC); }MACROE
		#define USB_HOST_VBUS_On()            MACROS{ PORTE |=  (1 << 7);        }MACROE
		#define USB_HOST_VBUS_Off()           MACROS{ PORTE &= ~(1 << 7);        }MACROE
	
#endif
