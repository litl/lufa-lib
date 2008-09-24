/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _RNDISETHERNET_H_
#define _RNDISETHERNET_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <string.h>

		#include "Descriptors.h"
		#include "RNDIS.h"
		#include "Ethernet.h"
		#include "TCP.h"
		#include "ARP.h"
		#include "Webserver.h"
		#include "Telnet.h"

		#include <MyUSB/Version.h>                        // Library Version Information
		#include <MyUSB/Common/ButtLoadTag.h>             // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>                // USB Functionality
		#include <MyUSB/Drivers/Board/LEDs.h>             // LEDs driver
		#include <MyUSB/Scheduler/Scheduler.h>            // Simple scheduler for task management

		#include <MyUSB/Drivers/AT90USBXXX/Serial_Stream.h>
	
	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_ConfigurationChanged);
		HANDLES_EVENT(USB_UnhandledControlPacket);

	/* Type Defines: */
		typedef struct
		{
			uint8_t  bmRequestType;
			uint8_t  bNotification;
			uint16_t wValue;
			uint16_t wIndex;
			uint16_t wLength;
		} USB_Notification_t;
		
	/* Tasks: */
		TASK(RNDIS_Task);
		TASK(Ethernet_Task);
		
#endif
