/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef HOST_H
#define HOST_H

	/* Private Macros */		
	#define USB_HOST_HostModeOn()   USBCON |= (1 << HOST)
	#define USB_HOST_HostModeOff()  USBCON &= ~(1 << HOST)

#endif
