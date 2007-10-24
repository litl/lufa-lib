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
		#include "MyUSB/Drivers/USB/USB.h"                // USB Functionality
		#include "MyUSB/Drivers/USB1287/Serial_Stream.h"  // USART Stream driver

	/* Event Catch List */
		HANDLES_EVENT(OnVBUSChange);
		HANDLES_EVENT(OnVBUSConnect);
		HANDLES_EVENT(OnVBUSDisconnect);
		HANDLES_EVENT(OnUSBConnect);
		HANDLES_EVENT(OnUSBDisconnect);
		HANDLES_EVENT(OnSuspend);
		HANDLES_EVENT(OnWakeUp);
		HANDLES_EVENT(OnReset);
		HANDLES_EVENT(OnUIDChange);
		HANDLES_EVENT(PowerOnFail);
		HANDLES_EVENT(HostError);

#endif
