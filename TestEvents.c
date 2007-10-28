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
	printf("EVENT: VBUS Change\r\n");
}

EVENT_HANDLER(USB_VBUSConnect)
{
	printf("EVENT: VBUS +\r\n");
}

EVENT_HANDLER(USB_VBUSDisconnect)
{
	printf("EVENT: VBUS -\r\n");
}

EVENT_HANDLER(USB_Connect)
{
	printf("EVENT: USB  +\r\n");
}

EVENT_HANDLER(USB_Disconnect)
{
	printf("EVENT: USB  -\r\n");
}

EVENT_HANDLER(USB_Suspend)
{
	printf("EVENT: USB Sleep\r\n");
}

EVENT_HANDLER(USB_WakeUp)
{
	printf("EVENT: USB Wakeup\r\n");
}

EVENT_HANDLER(USB_Reset)
{
	printf("EVENT: USB Reset\r\n");
}

EVENT_HANDLER(USB_UIDChange)
{
	printf("EVENT: UID Change\r\n");
}

EVENT_HANDLER(USB_PowerOnFail)
{
	printf("EVENT: Power On Fail\r\n");
	
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
	printf("EVENT: Host mode error\r\n");
	printf(" -- Error Code %d\r\n", ErrorCode);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	printf("EVENT: Ctrl Request\r\n");
	printf(" -- Request data %d\r\n", Request);
	printf(" -- Request type %d\r\n", RequestType);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	printf("EVENT: Make Endpoints\r\n");
}