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
	
	This bootloader is compatible with Atmel's FLIP application.
	However, it requires the use of Atmel's DFU drivers. You will
	need to install Atmel's FDU drivers prior to using this bootloader.
	
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
	  USB_USBTask();
	
	USB_ShutDown();
	
	Bicolour_SetLeds(BICOLOUR_LED1_ORANGE);
}

EVENT_HANDLER(USB_Connect)
{
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN);
}

EVENT_HANDLER(USB_Disconnect)
{
	RunBootloader = false;
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	switch (Request)
	{
		case DFU_GETSTATUS:
		
			break;
		default:
			Bicolour_SetLeds(BICOLOUR_LED1_ORANGE | BICOLOUR_LED2_ORANGE);
	}
}
