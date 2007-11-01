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
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
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
	puts_P(PSTR(EVENT_PREFIX ESC_BG_RED "Power On Fail\r\n"));
	
	switch (USB_CurrentMode)
	{
		case USB_MODE_DEVICE:
			puts_P(PSTR(" -- Mode DEVICE\r\n"));
			break;
		case USB_MODE_HOST:
			puts_P(PSTR(" -- Mode HOST\r\n"));
			break;
		default:
			puts_P(PSTR(" -- Mode N/A\r\n"));
			break;
	}
	
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);
}

EVENT_HANDLER(USB_HostError)
{
	puts_P(PSTR(EVENT_PREFIX ESC_BG_RED "Host mode error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	puts_P(PSTR(EVENT_PREFIX "Ctrl Request\r\n"));
	printf_P(PSTR(" -- Request data %d\r\n"), Request);
	printf_P(PSTR(" -- Request type %d\r\n"), RequestType);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	puts_P(PSTR(EVENT_PREFIX "Make Endpoints\r\n"));
}

