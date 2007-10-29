/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, VersiUSB_ 3
*/

#include "TestEvents.h"



EVENT_HANDLER(USB_VBUSChange)
{
	printf(EVENT_PREFIX "VBUS Change\r\n");
}

EVENT_HANDLER(USB_VBUSConnect)
{
	printf(EVENT_PREFIX "VBUS +\r\n");
}

EVENT_HANDLER(USB_VBUSDisconnect)
{
	printf(EVENT_PREFIX "VBUS -\r\n");
}

EVENT_HANDLER(USB_Connect)
{
	printf(EVENT_PREFIX "USB  +\r\n");
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_Disconnect)
{
	printf(EVENT_PREFIX "USB  -\r\n");
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_RED);
}

EVENT_HANDLER(USB_Suspend)
{
	printf(EVENT_PREFIX ESC_BG_YELLOW "USB Sleep\r\n");
	Bicolour_SetLeds(BICOLOUR_LED1_ORANGE | BICOLOUR_LED2_ORANGE);
}

EVENT_HANDLER(USB_WakeUp)
{
	printf(EVENT_PREFIX ESC_BG_GREEN "USB Wakeup\r\n");
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_Reset)
{
	printf(EVENT_PREFIX "USB Reset\r\n");
}

EVENT_HANDLER(USB_UIDChange)
{
	printf(EVENT_PREFIX "UID Change\r\n");
}

EVENT_HANDLER(USB_PowerOnFail)
{
	printf(EVENT_PREFIX ESC_BG_RED "Power On Fail\r\n");
	
	switch (USB_CurrentMode)
	{
		case USB_MODE_DEVICE:
			printf(" -- Mode DEVICE\r\n");
			break;
		case USB_MODE_HOST:
			printf(" -- Mode HOST\r\n");
			break;
		default:
			printf(" -- Mode N/A\r\n");
			break;
	}
	
	printf(" -- Error Code %d\r\n", ErrorCode);
}

EVENT_HANDLER(USB_HostError)
{
	printf(EVENT_PREFIX ESC_BG_RED "Host mode error\r\n");
	printf(" -- Error Code %d\r\n", ErrorCode);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	printf(EVENT_PREFIX "Ctrl Request\r\n");
	printf(" -- Request data %d\r\n", Request);
	printf(" -- Request type %d\r\n", RequestType);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	printf(EVENT_PREFIX "Make Endpoints\r\n");
}

