/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Denver Gingerich (denver [at] ossguy [dot] com)

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
	Keyboard demonstration application by Denver Gingerich.

	This example is based on the MyUSB Mouse demonstration application,
	written by Dean Camera.
*/

/*
	Keyboard demonstration application, using endpoint interrupts. This
	gives a simple reference application for implementing a USB Keyboard
	using the basic USB HID drivers in all modern OSes (i.e. no special
	drivers required). It is boot protocol compatible, and thus works under
	compatible BIOS as if it was a native keyboard (e.g. PS/2).
	
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

#include "KeyboardViaInt.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB KeyboardI App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
};

/* Global Variables */
bool      UsingReportProtocol = true;
uint8_t   IdleCount           = 0;
uint16_t  IdleMSRemaining     = 0;


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
	
	/* Millisecond timer initialization, with output compare interrupt enabled for the idle timing */
	OCR0A  = 0x7D;
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS01) | (1 << CS00));
	TIMSK0 = (1 << OCIE0A);

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
	/* Stop running keyboard reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Keyboard Keycode Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint IN interrupt ISR for the report endpoint */
	USB_INT_Enable(ENDPOINT_INT_IN);

	/* Setup Keyboard LED Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_LEDS_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_OUT, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint OUT interrupt ISR for the LED report endpoint */
	USB_INT_Enable(ENDPOINT_INT_OUT);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Handle HID Class specific requests */
	switch (bRequest)
	{
		case REQ_GetReport:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				USB_KeyboardReport_Data_t KeyboardReportData;

				/* Create the next keyboard report for transmission to the host */
				GetNextReport(&KeyboardReportData);

				/* Ignore report type and ID number value */
				Endpoint_Discard_Word();
				
				/* Ignore unused Interface number value */
				Endpoint_Discard_Word();

				/* Read in the number of bytes in the report to send to the host */
				uint16_t wLength = Endpoint_Read_Word_LE();
				
				/* If trying to send more bytes than exist to the host, clamp the value at the report size */
				if (wLength > sizeof(KeyboardReportData))
				  wLength = sizeof(KeyboardReportData);

				Endpoint_ClearSetupReceived();
	
				/* Write the report data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(&KeyboardReportData, wLength);
				
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

				/* Process the incomming LED report */
				ProcessLEDReport(LEDStatus);
			
				/* Clear the endpoint data */
				Endpoint_ClearSetupOUT();

				/* Wait until the host is ready to receive the request confirmation */
				while (!(Endpoint_IsSetupINReady()));
				
				/* Handshake the request by sending an empty IN packet */
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_GetProtocol:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				
				/* Write the current protocol flag to the host */
				Endpoint_Write_Byte(UsingReportProtocol);
				
				/* Send the flag to the host */
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_SetProtocol:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Read in the wValue parameter containing the new protocol mode */
				uint16_t wValue = Endpoint_Read_Word_LE();
				
				/* Set or clear the flag depending on what the host indicates that the current Protocol should be */
				UsingReportProtocol = (wValue != 0x0000);
				
				Endpoint_ClearSetupReceived();

				/* Send an empty packet to acknowedge the command */
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_SetIdle:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Read in the wValue parameter containing the idle period */
				uint16_t wValue = Endpoint_Read_Word_LE();
				
				Endpoint_ClearSetupReceived();
				
				/* Get idle period in MSB */
				IdleCount = (wValue >> 8);
				
				/* Send an empty packet to acknowedge the command */
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_GetIdle:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{		
				Endpoint_ClearSetupReceived();
				
				/* Write the current idle duration to the host */
				Endpoint_Write_Byte(IdleCount);
				
				/* Send the flag to the host */
				Endpoint_ClearSetupIN();
			}

			break;
	}
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	/* One millisecond has elapsed, decrement the idle time remaining counter if it has not already elapsed */
	if (IdleMSRemaining)
	  IdleMSRemaining--;
}

bool GetNextReport(USB_KeyboardReport_Data_t* ReportData)
{
	static uint8_t PrevJoyStatus = 0;
	uint8_t        JoyStatus_LCL        = Joystick_GetStatus();
	bool           InputChanged         = false;

	/* Clear the report contents */
	memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

	if (JoyStatus_LCL & JOY_UP)
	  ReportData->KeyCode[0] = 0x04; // A
	else if (JoyStatus_LCL & JOY_DOWN)
	  ReportData->KeyCode[0] = 0x05; // B

	if (JoyStatus_LCL & JOY_LEFT)
	  ReportData->KeyCode[0] = 0x06; // C
	else if (JoyStatus_LCL & JOY_RIGHT)
	  ReportData->KeyCode[0] = 0x07; // D

	if (JoyStatus_LCL & JOY_PRESS)
	  ReportData->KeyCode[0] = 0x08; // E
	  
	/* Check if the new report is different to the previous report */
	InputChanged = PrevJoyStatus ^ JoyStatus_LCL;

	/* Save the current joystick status for later comparison */
	PrevJoyStatus = JoyStatus_LCL;

	/* Return whether the new report is different to the previous report or not */
	return InputChanged;
}

void ProcessLEDReport(uint8_t LEDReport)
{
	uint8_t LEDMask   = LEDS_LED2;
	
	if (LEDReport & 0x01) // NUM Lock
	  LEDMask |= LEDS_LED1;
	
	if (LEDReport & 0x02) // CAPS Lock
	  LEDMask |= LEDS_LED3;

	if (LEDReport & 0x04) // SCROLL Lock
	  LEDMask |= LEDS_LED4;

	/* Set the status LEDs to the current Keyboard LED status */
	LEDs_SetAllLEDs(LEDMask);
}

ISR(ENDPOINT_PIPE_vect)
{
	/* Save previously selected endpoint before selecting a new endpoint */
	uint8_t PrevSelectedEndpoint = Endpoint_GetCurrentEndpoint();

	/* Check if keyboard endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(KEYBOARD_EPNUM))
	{
		USB_KeyboardReport_Data_t KeyboardReportData;
		bool                      SendReport;
	
		/* Create the next keyboard report for transmission to the host */
		SendReport = GetNextReport(&KeyboardReportData);
	
		/* Check if the idle period is set and has elapsed */
		if (IdleCount && !(IdleMSRemaining))
		{
			/* Idle period elapsed, indicate that a report must be sent */
			SendReport = true;
			
			/* Reset the idle time remaining counter, must multiply by 4 to get the duration in milliseconds */
			IdleMSRemaining = (IdleCount << 2);
		}
	
		/* Select the Keyboard Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

		/* Clear the endpoint IN interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_IN);

		/* Clear the Keyboard Report endpoint interrupt */
		Endpoint_ClearEndpointInterrupt(KEYBOARD_EPNUM);

		/* Check to see if a report should be issued */
		if (SendReport)
		{
			/* Write Keyboard Report Data */
			Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));
		}

		/* Handshake the IN Endpoint - send the data to the host */
		Endpoint_ClearCurrentBank();		
	}

	/* Check if Keyboard LED status Endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(KEYBOARD_LEDS_EPNUM))
	{
		/* Select the Keyboard LED Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_LEDS_EPNUM);

		/* Clear the endpoint OUT interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_OUT);

		/* Clear the Keyboard LED Report endpoint interrupt */
		Endpoint_ClearEndpointInterrupt(KEYBOARD_LEDS_EPNUM);

		/* Read in the LED report from the host */
		uint8_t LEDStatus = Endpoint_Read_Byte();

		/* Process the incomming LED report */
		ProcessLEDReport(LEDStatus);

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_ClearCurrentBank();
	}
	
	/* Restore previously selected endpoint */
	Endpoint_SelectEndpoint(PrevSelectedEndpoint);
}

