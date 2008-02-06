/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

	/* Includes: */
		#include "Descriptors.h"
		
		#include <stdbool.h>
		
		#include <MyUSB/Drivers/USB/USB.h>
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>

	/* Event Handlers: */
		HANDLES_EVENT(USB_CreateEndpoints);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		
#endif
