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

	/* Public Macros */
		#define USB_HOST_AUTOVBUS             (0 << 1)
		#define USB_HOST_MANUALVBUS           (1 << 1)

		#define USB_HOST_VBUS_On()            MACROS{ PORTE |=  (1 << 7);       }MACROE
		#define USB_HOST_VBUS_Off()           MACROS{ PORTE &= ~(1 << 7);       }MACROE

	/* Enums */
		enum USB_Host_States
		{
			HOST_STATE_Unattached       = 0,
			HOST_STATE_Powered          = 2,
			HOST_STATE_Attached         = 1,
			HOST_STATE_Default          = 3,
			HOST_STATE_Addressed        = 4,
			HOST_STATE_Configured       = 5,
			HOST_STATE_Ready            = 6,
		};
		
		enum USB_Host_ErrorCodes
		{
			HOST_ERROR_VBusVoltageDip   = 0;
		};
		
	/* Private Macros */
		#define USB_HOST_HostModeOn()              MACROS{ USBCON |= (1 << HOST);     }MACROE
		#define USB_HOST_HostModeOff()             MACROS{ USBCON &= ~(1 << HOST);    }MACROE

		#define USB_HOST_EnableManualVBUS()        MACROS{ UHWCON &= ~(1 << UVCONE); OTGCON |=  (1 << VBUSHWC); DDRE |= (1 << 7); }MACROE
		#define USB_HOST_DisableManualVBUS()       MACROS{ OTGCON &= ~(1 << VBUSHWC); }MACROE

		#define USB_HOST_AutoVBUS_On()             MACROS{ UHWCON |= (1 << UVCONE); OTGCON |= (1 << VBUSREQ); }MACROE
		#define USB_HOST_AutoVBUS_Off()            MACROS{ OTGCON |= (1 << VBUSRQC); }MACROE 
#endif
