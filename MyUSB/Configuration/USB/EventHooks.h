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
		#define USB_EVENT_OnVBUSChange()	                        { printf("EVENT: VBus !=\r\n"); }
		#define USB_EVENT_OnVBUSConnect()                           { printf("EVENT: VBus +\r\n"); }
		#define USB_EVENT_OnVBUSDisconnect()                        { printf("EVENT: VBus -\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_ORANGE); }
		#define USB_EVENT_OnUSBConnect()                            { printf("EVENT: USB  +\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_RED   | BICOLOUR_LED2_RED); }
		#define USB_EVENT_OnUSBDisconnect()                         { printf("EVENT: USB  -\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_ORANGE); }
		#define USB_EVENT_OnSuspend()                               { printf("EVENT: USB Sleep\r\n"); Bicolour_SetLeds(BICOLOUR_ALL_LEDS); }
		#define USB_EVENT_OnWakeUp()                                { printf("EVENT: USB Wakeup\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN); }
		#define USB_EVENT_OnReset()                                 { printf("EVENT: USB Reset\r\n"); Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN); }
		#define USB_EVENT_OnUIDChange()                             { printf("EVENT: UID Change\r\n"); }
		#define USB_EVENT_PowerOnFail(mode, errno)                  { printf("EVENT: Power on fail\r\n"); printf(" -- Mode %s\r\n", mode); printf(" -- Error Code %d\r\n", errno); Bicolour_SetLeds(BICOLOUR_LED1_RED); }

#endif
