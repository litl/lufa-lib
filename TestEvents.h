/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef TESTEVENTS_H
#define TESTEVENTS_H

	/* Includes */
		#include <avr/io.h>

		#include "MyUSB/Drivers/USB/USB.h"                // USB Functionality
		#include "MyUSB/Drivers/USB1287/Serial_Stream.h"  // USART Stream driver

	/* Event Catch List */
		HANDLES_EVENT(USB_VBUSChange);
		HANDLES_EVENT(USB_VBUSConnect);
		HANDLES_EVENT(USB_VBUSDisconnect);
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_Suspend);
		HANDLES_EVENT(USB_WakeUp);
		HANDLES_EVENT(USB_Reset);
		HANDLES_EVENT(USB_UIDChange);
		HANDLES_EVENT(USB_PowerOnFail);
		HANDLES_EVENT(USB_HostError);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		HANDLES_EVENT(USB_CreateEndpoints);

#endif
