/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/*
	Portions of this example is based on the MyUSB Keyboard demonstration
	application, written by Denver Gingerich.
*/

/*
	Keyboard/Mouse demonstration application. This gives a simple reference
	application for implementing a composite device containing both USB Keyboard
	and USB Mouse functionality using the basic USB HID drivers in all modern OSes
	(i.e. no special drivers required). This example uses two seperate HID
	interfaces for each function. It is boot protocol compatible, and thus works under
	compatible BIOS as if it was a native keyboard and mouse (e.g. PS/2).
	
	On startup the system will automatically enumerate and function
	as a keyboard when the USB connection to a host is present and the HWB is not
	pressed. When enabled, manipulate the joystick to send the letters
	a, b, c, d and e. See the USB HID documentation for more information
	on sending keyboard event and keypresses.
	
	When the HWB is pressed, the mouse mode is enabled. When enabled, move the
	joystick to move the pointer, and push the joystick inwards to simulate a
	left-button click.
*/

/*
	USB Mode:           Device
	USB Class:          Human Interface Device (HID)
	USB Subclass:       HID
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables 
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/

#include "KeyboardMouse.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB MouseKBD App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask               , TaskStatus: TASK_STOP },
	{ Task: USB_Mouse                 , TaskStatus: TASK_RUN },
	{ Task: USB_Keyboard              , TaskStatus: TASK_RUN },
};

/* Global Variables */
USB_KeyboardReport_Data_t KeyboardReportData;
USB_MouseReport_Data_t    MouseReportData;
bool                      UsingReportProtocol = true;


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

	/* Default to report protocol on connect */
	UsingReportProtocol = true;
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running HID reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Keyboard Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_IN_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, HID_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Setup Keyboard LED Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_OUT_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_OUT, HID_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Setup Mouse Report Endpoint */
	Endpoint_ConfigureEndpoint(MOUSE_IN_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, HID_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	uint8_t* ReportData;
	uint8_t  ReportSize;

	/* Handle HID Class specific requests */
	switch (bRequest)
	{
		case REQ_GetReport:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_Ignore_Word();
			
				uint16_t wIndex = Endpoint_Read_Word_LE();
				
				/* Determine if it is the mouse or the keyboard data that is being requested */
				if (!(wIndex))
				{
					ReportData = (uint8_t*)&KeyboardReportData;
					ReportSize = sizeof(KeyboardReportData);
				}
				else
				{
					ReportData = (uint8_t*)&MouseReportData;
					ReportSize = sizeof(MouseReportData);
				}
				
				/* Ignore unused Interface number value */
				Endpoint_Discard_Word();

				/* Read in the number of bytes in the report to send to the host */
				uint16_t wLength = Endpoint_Read_Word_LE();
				
				/* If trying to send more bytes than exist to the host, clamp the value at the report size */
				if (wLength > ReportSize)
				  wLength = ReportSize;

				Endpoint_ClearSetupReceived();
	
				/* Write the report data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(ReportData, wLength);

				/* Clear the report data afterwards */
				memset(ReportData, 0, ReportSize);
				
				/* Finalize the transfer, acknowedge the host error or success OUT transfer */
				Endpoint_ClearSetupOUT();
			}
		
			break;
		case REQ_SetReport:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				
				/* Wait until the LED report has been sent by the host */
				while (!(Endpoint_IsSetupOUTReceived()));

				/* Read in the LED report from the host */
				uint8_t LEDStatus = Endpoint_Read_Byte();
				uint8_t LEDMask   = LEDS_LED2;
				
				if (LEDStatus & 0x01) // NUM Lock
				  LEDMask |= LEDS_LED1;
				
				if (LEDStatus & 0x02) // CAPS Lock
				  LEDMask |= LEDS_LED3;

				if (LEDStatus & 0x04) // SCROLL Lock
				  LEDMask |= LEDS_LED4;

				/* Set the status LEDs to the current HID LED status */
				LEDs_SetAllLEDs(LEDMask);

				/* Clear the endpoint data */
				Endpoint_ClearSetupOUT();

				/* Wait until the host is ready to receive the request confirmation */
				while (!(Endpoint_IsSetupINReady()));
				
				/* Handshake the request by sending an empty IN packet */
				Endpoint_ClearSetupIN();
			}
			
			break;
	}
}

TASK(USB_Keyboard)
{
	uint8_t JoyStatus_LCL = Joystick_GetStatus();

	/* Check if HWB is not pressed, if so mouse mode enabled */
	if (!(HWB_GetStatus()))
	{
		if (JoyStatus_LCL & JOY_UP)
		  KeyboardReportData.KeyCode[0] = 0x04; // A
		else if (JoyStatus_LCL & JOY_DOWN)
		  KeyboardReportData.KeyCode[0] = 0x05; // B

		if (JoyStatus_LCL & JOY_LEFT)
		  KeyboardReportData.KeyCode[0] = 0x06; // C
		else if (JoyStatus_LCL & JOY_RIGHT)
		  KeyboardReportData.KeyCode[0] = 0x07; // D

		if (JoyStatus_LCL & JOY_PRESS)
		  KeyboardReportData.KeyCode[0] = 0x08; // E
	}
	
	/* Check if the USB system is connected to a host and report protocol mode is enabled */
	if (USB_IsConnected && UsingReportProtocol)
	{
		/* Select the Keyboard Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_IN_EPNUM);

		/* Check if Keyboard Endpoint Ready for Read/Write */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Write Keyboard Report Data */
			Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));

			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_ClearCurrentBank();

			/* Clear the report data afterwards */
			memset(&KeyboardReportData, 0, sizeof(KeyboardReportData));
		}

		/* Select the Keyboard LED Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_OUT_EPNUM);

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
			Endpoint_ClearCurrentBank();
		}
	}
}

TASK(USB_Mouse)
{
	uint8_t JoyStatus_LCL = Joystick_GetStatus();

	/* Check if HWB is pressed, if so mouse mode enabled */
	if (HWB_GetStatus())
	{
		if (JoyStatus_LCL & JOY_UP)
		  MouseReportData.Y =  1;
		else if (JoyStatus_LCL & JOY_DOWN)
		  MouseReportData.Y = -1;

		if (JoyStatus_LCL & JOY_RIGHT)
		  MouseReportData.X =  1;
		else if (JoyStatus_LCL & JOY_LEFT)
		  MouseReportData.X = -1;

		if (JoyStatus_LCL & JOY_PRESS)
		  MouseReportData.Button  = (1 << 0);
	}

	/* Check if the USB system is connected to a host and report protocol mode is enabled */
	if (USB_IsConnected && UsingReportProtocol)
	{
		/* Select the Mouse Report Endpoint */
		Endpoint_SelectEndpoint(MOUSE_IN_EPNUM);

		/* Check if Mouse Endpoint Ready for Read/Write */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Write Mouse Report Data */
			Endpoint_Write_Stream_LE(&MouseReportData, sizeof(MouseReportData));

			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_ClearCurrentBank();

			/* Clear the report data afterwards */
			memset(&MouseReportData, 0, sizeof(MouseReportData));
		}
	}
}
