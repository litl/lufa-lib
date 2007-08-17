/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef EVENTHOOKS_H
#define EVENTHOOKS_H

	/* Private Macros */
		#define NO_EVENT_HOOK                                       asm ("")

	/* Place any includes, defines and such used by your user-application hooks here. */
		#include "../../Drivers/USB1287/Serial_Stream.h"
		#include "../../Drivers/USBKEY/Bicolour.h"

	/* Configure user-application hooks here. */
		#define USB_EVENT_OnVBUSChange()	                        { printf("EVENT: VBus Change\r\n"); }
		#define USB_EVENT_OnVBUSConnect()                           { printf("EVENT: VBus Connect\r\n"); }
		#define USB_EVENT_OnVBUSDisconnect()                        { printf("EVENT: VBus Disonnect\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_ORANGE); }
		#define USB_EVENT_OnUSBConnect()                            { printf("EVENT: USB Connect\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_RED   | BICOLOUR_LED2_RED); }
		#define USB_EVENT_OnUSBDisconnect()                         { printf("EVENT: USB Disconnect\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_ORANGE); }
		#define USB_EVENT_OnSuspend()                               { printf("EVENT: USB Suspend\r\n"); Bicolour_SetLeds(BICOLOUR_ALL_LEDS); }
		#define USB_EVENT_OnWakeUp()                                { printf("EVENT: USB Wakeup\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN); }
		#define USB_EVENT_OnReset()                                 { printf("EVENT: USB Reset\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN); }
		#define USB_EVENT_OnUIDChange()                             { printf("EVENT: UID Change\r\n"); }
#endif
