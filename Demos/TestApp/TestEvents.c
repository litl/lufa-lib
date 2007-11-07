/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "TestEvents.h"

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
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_ORANGE);
}

EVENT_HANDLER(USB_Disconnect)
{
	puts_P(PSTR(EVENT_PREFIX "USB  -\r\n"));
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_RED);
}

EVENT_HANDLER(USB_Suspend)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_YELLOW "USB Sleep\r\n"));
	Bicolour_SetLeds(BICOLOUR_LED1_ORANGE | BICOLOUR_LED2_ORANGE);
}

EVENT_HANDLER(USB_WakeUp)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_GREEN "USB Wakeup\r\n"));
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_Reset)
{
	puts_P(PSTR(EVENT_PREFIX "USB Reset\r\n"));
}

EVENT_HANDLER(USB_UIDChange)
{
	puts_P(PSTR(EVENT_PREFIX "UID Change\r\n"));
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

	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);

	for (;;);
}

EVENT_HANDLER(USB_HostError)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_RED "Host Mode Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);

	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	for (;;);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	puts_P(PSTR(EVENT_PREFIX "Ctrl Request\r\n"));
	printf_P(PSTR(" -- Req Data %d\r\n"), Request);
	printf_P(PSTR(" -- Req Type %d\r\n"), RequestType);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	puts_P(PSTR(EVENT_PREFIX "Make Endpoints\r\n"));

	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_GREEN "Device +\r\n"));
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_YELLOW "Device -\r\n"));
}
