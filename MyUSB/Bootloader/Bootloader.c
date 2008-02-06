/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	MyUSB USB Bootloader. This bootloader enumerates to the host
	as a DFU Class device, allowing for DFU-compatible programming
	software to load firmware onto the AVR.
	
	*** WORK IN PROGRESS - NOT CURRENTLY FUNCTIONING ***
*/

#include "Bootloader.h"

bool RunBootloader = true;

int main (void)
{
	Bicolour_Init();
	
	Bicolour_SetLeds(BICOLOUR_LED1_RED);

	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	while (RunBootloader)
	{
		USB_USBTask();
	}
	
	USB_ShutDown();
	
	Bicolour_SetLeds(BICOLOUR_LED1_ORANGE);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	
}
