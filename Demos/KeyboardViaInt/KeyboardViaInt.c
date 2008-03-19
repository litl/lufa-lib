/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Keyboard demonstration application by Denver Gingerich.

	This example is based on the MyUSB Mouse demonstration application,
	written by Dean Camera.
*/

/*
	Keyboard demonstration application, using endpoint interrupts. This
	gives a simple reference application for implementing a USB Keyboard
	using the basic USB HID drivers in all modern OSes (i.e. no special
	drivers required).
	
	On startup the system will automatically enumerate and function
	as a keyboard when the USB connection to a host is present. To use
	the keyboard example, manipulate the joystick to send the letters
	a, b, c, d and e. See the USB HID documentation for more information
	on sending keyboard event and keypresses.
*/

#include "KeyboardViaInt.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB KeyboardI App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
};

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	Joystick_Init();
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();
	
	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_Connect)
{
	/* Start USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);

	/* Red/green to indicate USB enumerating */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running keyboard reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup Keyboard Report Endpoints */
	Endpoint_ConfigureEndpoint(KEYBOARD_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint IN interrupt ISR for the report endpoint */
	USB_INT_Enable(ENDPOINT_INT_IN);

	/* Setup Keyboard Report Endpoints */
	Endpoint_ConfigureEndpoint(KEYBOARD_LEDS_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_OUT, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint OUT interrupt ISR for the LED report endpoint */
	USB_INT_Enable(ENDPOINT_INT_OUT);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);	
}

ISR(ENDPOINT_PIPE_vect)
{
	/* Check if keyboard endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(KEYBOARD_EPNUM))
	{
		USB_KeyboardReport_Data_t KeyboardReportData = {Modifier: 0, KeyCode: 0};
		uint8_t                   JoyStatus_LCL      = Joystick_GetStatus();

		if (JoyStatus_LCL & JOY_UP)
		  KeyboardReportData.KeyCode = 0x04; // A
		else if (JoyStatus_LCL & JOY_DOWN)
		  KeyboardReportData.KeyCode = 0x05; // B

		if (JoyStatus_LCL & JOY_LEFT)
		  KeyboardReportData.KeyCode = 0x06; // C
		else if (JoyStatus_LCL & JOY_RIGHT)
		  KeyboardReportData.KeyCode = 0x07; // D

		if (JoyStatus_LCL & JOY_PRESS)
		  KeyboardReportData.KeyCode = 0x08; // E

		/* Clear the Keyboard Report endpoint interrupt and select the endpoint */
		Endpoint_ClearEndpointInterrupt(KEYBOARD_EPNUM);
		Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

		/* Write Keyboard Report Data */
		Endpoint_Write_Byte(KeyboardReportData.Modifier);
		Endpoint_Write_Byte(0x00);
		Endpoint_Write_Byte(KeyboardReportData.KeyCode);			
		Endpoint_Write_Byte(0x00);
		Endpoint_Write_Byte(0x00);
		Endpoint_Write_Byte(0x00);
		Endpoint_Write_Byte(0x00);
		Endpoint_Write_Byte(0x00);

		/* Handshake the IN Endpoint - send the data to the host */
		Endpoint_FIFOCON_Clear();
		
		/* Clear the endpoint IN interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_IN);
	}

	/* Check if Keyboard LED status Endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(KEYBOARD_LEDS_EPNUM))
	{
		/* Clear the Keyboard LED Report endpoint interrupt and select the endpoint */
		Endpoint_ClearEndpointInterrupt(KEYBOARD_LEDS_EPNUM);
		Endpoint_SelectEndpoint(KEYBOARD_LEDS_EPNUM);

		/* Read in the LED report from the host */
		uint8_t LEDStatus = Endpoint_Read_Byte();
		uint8_t LEDMask   = BICOLOUR_LED1_GREEN;
		
		if (LEDStatus & 0x01) // NUM Lock
		  LEDMask |= BICOLOUR_LED2_RED;
		
		if (LEDStatus & 0x02) // CAPS Lock
		  LEDMask |= BICOLOUR_LED1_RED;

		if (LEDStatus & 0x04) // SCROLL Lock
		  LEDMask |= BICOLOUR_LED2_GREEN;

		/* Set the status LEDs to the current Keyboard LED status */
		Bicolour_SetLeds(LEDMask);

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_FIFOCON_Clear();

		/* Clear the endpoint OUT interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_OUT);
	}
}

