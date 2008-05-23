/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#define INCLUDE_FROM_TESTEVENTS_C
#include "TestEvents.h"

static void Abort_Program(void)
{
	USB_ShutDown();

	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);

	puts_P(PSTR(ESC_INVERSE_ON "\r\n**PROGRAM ABORT**"));
	for (;;);
}

EVENT_HANDLER(USB_VBUSChange)
{
	puts_P(PSTR(EVENT_PREFIX "VBUS Change\r\n"));
}

EVENT_HANDLER(USB_VBUSConnect)
{
	puts_P(PSTR(EVENT_PREFIX "VBUS +\r\n"));
}

EVENT_HANDLER(USB_VBUSDisconnect)
{
	puts_P(PSTR(EVENT_PREFIX "VBUS -\r\n"));
}

EVENT_HANDLER(USB_Connect)
{
	puts_P(PSTR(EVENT_PREFIX "USB  +\r\n"));
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED3 | LEDS_LED4);
	
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
}

EVENT_HANDLER(USB_Disconnect)
{
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	puts_P(PSTR(EVENT_PREFIX "USB  -\r\n"));
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED3 | LEDS_LED3);
}

EVENT_HANDLER(USB_Suspend)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_YELLOW "USB Sleep\r\n"));
	LEDs_SetAllLEDs(LEDS_ALL_LEDS);
}

EVENT_HANDLER(USB_WakeUp)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_GREEN "USB Wakeup\r\n"));
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
}

EVENT_HANDLER(USB_Reset)
{
	puts_P(PSTR(EVENT_PREFIX "USB Reset\r\n"));
}

EVENT_HANDLER(USB_UIDChange)
{
	char* ModeStrPtr;

	puts_P(PSTR(EVENT_PREFIX "UID Change\r\n"));

	if (USB_CurrentMode == USB_MODE_DEVICE)
	  ModeStrPtr = PSTR("HOST");
	else if (USB_CurrentMode == USB_MODE_HOST)
	  ModeStrPtr = PSTR("DEVICE");
	else
	  ModeStrPtr = PSTR("N/A");
	
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED3);

	printf_P(PSTR(" -- New Mode %S\r\n"), ModeStrPtr);
}

EVENT_HANDLER(USB_PowerOnFail)
{
	char* ModeStrPtr;
	
	puts_P(PSTR(EVENT_PREFIX ESC_BG_RED "Power On Fail\r\n"));

	if (USB_CurrentMode == USB_MODE_DEVICE)
	  ModeStrPtr = PSTR("DEVICE");
	else if (USB_CurrentMode == USB_MODE_HOST)
	  ModeStrPtr = PSTR("HOST");
	else
	  ModeStrPtr = PSTR("N/A");
	
	printf_P(PSTR(" -- Mode %S\r\n"), ModeStrPtr);
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);

	Abort_Program();
}

EVENT_HANDLER(USB_HostError)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_RED "Host Mode Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);

	Abort_Program();
}

EVENT_HANDLER(USB_DeviceEnumerationFailed)
{
	puts_P(PSTR(ESC_BG_RED "Dev Enum Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);
}

EVENT_HANDLER(USB_DeviceError)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_RED "Device Mode Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);

	Abort_Program();
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	puts_P(PSTR(EVENT_PREFIX "Ctrl Request\r\n"));
	printf_P(PSTR(" -- Req Data %d\r\n"), Request);
	printf_P(PSTR(" -- Req Type %d\r\n"), RequestType);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	puts_P(PSTR(EVENT_PREFIX "Configuration Number Changed\r\n"));

	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_GREEN "Device +\r\n"));

	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_YELLOW "Device -\r\n"));
}

EVENT_HANDLER(USB_DeviceEnumerationComplete)
{
	puts_P(PSTR(EVENT_PREFIX "Device Enumeration Complete"));
}
