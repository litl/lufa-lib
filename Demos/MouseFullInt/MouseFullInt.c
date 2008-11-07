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
	Mouse demonstration application, using endpoint interrupts. This
	gives a simple reference application for implementing a USB Mouse
	using the basic USB HID drivers in all modern OSes (i.e. no special
	drivers required). Control requests are also fully interrupt driven.
	It is boot protocol compatible, and thus works under compatible BIOS
	as if it was a native mouse (e.g. PS/2).
	
	On startup the system will automatically enumerate and function
	as a mouse when the USB connection to a host is present. To use
	the mouse, move the joystick to move the pointer, and push the
	joystick inwards to simulate a left-button click. The HWB serves as
	the right mouse button.
*/

/*
	USB Mode:           Device
	USB Class:          Human Interface Device (HID)
	USB Subclass:       Mouse
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables 
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/

#include "MouseFullInt.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB MouseFI App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

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
	HWB_Init();
	
	/* Millisecond timer initialization, with output compare interrupt enabled for the idle timing */
	OCR0A  = 0x7D;
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS01) | (1 << CS00));
	TIMSK0 = (1 << OCIE0A);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
	
	/* Initialize USB Subsystem */
	USB_Init();

	/* Main program code loop */
	for (;;)
	{
		/* No main code -- all USB code is interrupt driven */
	}
}

EVENT_HANDLER(USB_Connect)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED4);

	/* Default to report protocol on connect */
	UsingReportProtocol = true;
}

EVENT_HANDLER(USB_Reset)
{
	/* Select the control endpoint */
	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);

	/* Enable the endpoint SETUP interrupt ISR for the control endpoint */
	USB_INT_Enable(ENDPOINT_INT_SETUP);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Mouse Report Endpoint */
	Endpoint_ConfigureEndpoint(MOUSE_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, MOUSE_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint IN interrupt ISR for the report endpoint */
	USB_INT_Enable(ENDPOINT_INT_IN);

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
				USB_MouseReport_Data_t MouseReportData;

				/* Create the next mouse report for transmission to the host */
				GetNextReport(&MouseReportData);

				/* Ignore report type and ID number value */
				Endpoint_Discard_Word();
				
				/* Ignore unused Interface number value */
				Endpoint_Discard_Word();

				/* Read in the number of bytes in the report to send to the host */
				uint16_t wLength = Endpoint_Read_Word_LE();
				
				/* If trying to send more bytes than exist to the host, clamp the value at the report size */
				if (wLength > sizeof(MouseReportData))
				  wLength = sizeof(MouseReportData);

				Endpoint_ClearSetupReceived();
	
				/* Write the report data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(&MouseReportData, wLength);
				
				/* Clear the report data afterwards */
				memset(&MouseReportData, 0, sizeof(MouseReportData));

				/* Finalize the transfer, acknowedge the host error or success OUT transfer */
				Endpoint_ClearSetupOUT();
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

bool GetNextReport(USB_MouseReport_Data_t* ReportData)
{
	static uint8_t PrevJoyStatus = 0;
	uint8_t        JoyStatus_LCL        = Joystick_GetStatus();
	bool           InputChanged         = false;

	/* Clear the report contents */
	memset(ReportData, 0, sizeof(USB_MouseReport_Data_t));

	if (JoyStatus_LCL & JOY_UP)
	  ReportData->Y =  1;
	else if (JoyStatus_LCL & JOY_DOWN)
	  ReportData->Y = -1;

	if (JoyStatus_LCL & JOY_RIGHT)
	  ReportData->X =  1;
	else if (JoyStatus_LCL & JOY_LEFT)
	  ReportData->X = -1;

	if (JoyStatus_LCL & JOY_PRESS)
	  ReportData->Button  = (1 << 0);
	  
	if (HWB_GetStatus())
	  ReportData->Button |= (1 << 1);
	  
	/* Check if the new report is different to the previous report */
	InputChanged = PrevJoyStatus ^ JoyStatus_LCL;

	/* Save the current joystick status for later comparison */
	PrevJoyStatus = JoyStatus_LCL;

	/* Return whether the new report is different to the previous report or not */
	return InputChanged;
}

ISR(ENDPOINT_PIPE_vect)
{
	/* Check if the control endpoint has received a request */
	if (Endpoint_HasEndpointInterrupted(ENDPOINT_CONTROLEP))
	{
		/* Clear the endpoint interrupt */
		Endpoint_ClearEndpointInterrupt(ENDPOINT_CONTROLEP);

		/* Process the control request */
		USB_USBTask();

		/* Handshake the endpoint setup interrupt - must be after the call to USB_USBTask() */
		USB_INT_Clear(ENDPOINT_INT_SETUP);
	}

	/* Check if mouse endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(MOUSE_EPNUM))
	{
		USB_MouseReport_Data_t MouseReportData;
		bool                   SendReport = true;
		
		/* Create the next mouse report for transmission to the host */
		GetNextReport(&MouseReportData);
		
		/* Check if the idle period is set*/
		if (IdleCount)
		{
			/* Determine if the idle period has elapsed */
			if (!(IdleMSRemaining))
			{
				/* Reset the idle time remaining counter, must multiply by 4 to get the duration in milliseconds */
				IdleMSRemaining = (IdleCount << 2);		
			}
			else
			{
				/* Idle period not elapsed, indicate that a report must not be sent */
				SendReport = false;
			}
		}
		
		/* Check to see if a report should be issued */
		if (SendReport)
		{
			/* Select the Mouse Report Endpoint */
			Endpoint_SelectEndpoint(MOUSE_EPNUM);

			/* Clear the endpoint IN interrupt flag */
			USB_INT_Clear(ENDPOINT_INT_IN);

			/* Clear the Mouse Report endpoint interrupt and select the endpoint */
			Endpoint_ClearEndpointInterrupt(MOUSE_EPNUM);

			/* Write Mouse Report Data */
			Endpoint_Write_Stream_LE(&MouseReportData, sizeof(MouseReportData));

			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_ClearCurrentBank();
		}
	}
}
