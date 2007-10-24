/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "TestEvents.h"

EVENT_HANDLER(OnVBUSChange)
{
	printf("EVENT: VBus !=\r\n");
}

EVENT_HANDLER(OnVBUSConnect)
{
	printf("EVENT: VBus +\r\n");
}

EVENT_HANDLER(OnVBUSDisconnect)
{
	printf("EVENT: VBus -\r\n");
}

EVENT_HANDLER(OnUSBConnect)
{
	printf("EVENT: USB  +\r\n");
}

EVENT_HANDLER(OnUSBDisconnect)
{
	printf("EVENT: USB  -\r\n");
}

EVENT_HANDLER(OnSuspend)
{
	printf("EVENT: USB Sleep\r\n");
}

EVENT_HANDLER(OnWakeUp)
{
	printf("EVENT: USB Wakeup\r\n");
}

EVENT_HANDLER(OnReset)
{
	printf("EVENT: USB Reset\r\n");
}

EVENT_HANDLER(OnUIDChange)
{
	printf("EVENT: UID Change\r\n");
}

EVENT_HANDLER(PowerOnFail)
{
	printf("EVENT: Power on fail\r\n");
	
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

EVENT_HANDLER(HostError)
{
	printf("EVENT: Host mode error\r\n");
	printf(" -- Error Code %d\r\n", ErrorCode);
}
