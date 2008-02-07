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
		
		#include <avr/io.h>
		#include <avr/boot.h>
		#include <stdbool.h>
		
		#include <MyUSB/Drivers/USB/USB.h>
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>
		
	/* Macros: */
		#define DFU_DETATCH          0x00
		#define DFU_DNLOAD           0x01
		#define DFU_UPLOAD           0x02
		#define DFU_GETSTATUS        0x03
		#define DFU_CLRSTATUS        0x04
		#define DFU_GETSTATE         0x05
		#define DFU_ABORT            0x06

	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		
#endif
