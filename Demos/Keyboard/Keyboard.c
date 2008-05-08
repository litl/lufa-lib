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
	Keyboard demonstration application. This gives a simple reference
	application for implementing a USB Keyboard using the basic USB HID
	drivers in all modern OSes (i.e. no special drivers required).
	
	On startup the system will automatically enumerate and function
	as a keyboard when the USB connection to a host is present. To use
	the keyboard example, manipulate the joystick to send the letters
	a, b, c, d and e. See the USB HID documentation for more information
	on sending keyboard event and keypresses.
*/

/*
	USB Mode:           Device
	USB Class:          Human Interface Device (HID)
	USB Subclass:       Keyboard
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables 
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/

#include "Keyboard.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB Keyboard App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Keyboard_Report  , TaskStatus: TASK_STOP },
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
	LEDs_Init();
	
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
	
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

	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED4);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running keyboard reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_Keyboard_Report, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup Keyboard Keycode Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Setup Keyboard LED Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_LEDS_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_OUT, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
	
	/* Start Keyboard reporting task */
	Scheduler_SetTaskMode(USB_Keyboard_Report, TASK_RUN);
}

TASK(USB_Keyboard_Report)
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

	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		/* Select the Keyboard Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

		/* Check if Keyboard Endpoint Ready for Read/Write */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Write Keyboard Report Data */
			Endpoint_Write_Byte(KeyboardReportData.Modifier);
			Endpoint_Write_Byte(0x00);
			Endpoint_Write_Byte(KeyboardReportData.KeyCode);

			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_FIFOCON_Clear();
		}

		/* Select the Keyboard LED Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_LEDS_EPNUM);

		/* Check if Keyboard LED Endpoint Ready for Read/Write */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Read in the LED report from the host */
			uint8_t LEDStatus = Endpoint_Read_Byte();
			uint8_t LEDMask   = LEDS_LED2;
			
			if (LEDStatus & 0x01) // NUM Lock
			  LEDMask |= LEDS_LED1;
			
			if (LEDStatus & 0x02) // CAPS Lock
			  LEDMask |= LEDS_LED3;

			if (LEDStatus & 0x04) // SCROLL Lock
			  LEDMask |= LEDS_LED4;

			/* Set the status LEDs to the current Keyboard LED status */
			LEDs_SetAllLEDs(LEDMask);

			/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
			Endpoint_FIFOCON_Clear();
		}
	}
}

