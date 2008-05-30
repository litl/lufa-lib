/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Joystick demonstration application. This gives a simple reference
	application for implementing a USB Keyboard device, for USB Joysticks
	using the standard Keyboard HID profile.
	
	This device will show up as a generic joystick device, with two buttons.
	Pressing the joystick inwards is the first button, and the HWB button
	is the second.
	
	Moving the joystick on the selected board moves the joystick location on
	the host computer.
	
	Currently only single interface joysticks are supported.
*/

/*
	USB Mode:           Device
	USB Class:          Human Interface Device (HID)
	USB Subclass:       Joystick
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables 
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/

#include "Joystick.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB Joystick App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Joystick_Report  , TaskStatus: TASK_STOP },
};

/* Global Variables */
USB_JoystickReport_Data_t JoystickReportData = {Button: 0, X: 0, Y: 0};

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
	/* Stop running joystick reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_Joystick_Report, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Joystick Report Endpoint */
	Endpoint_ConfigureEndpoint(JOYSTICK_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, JOYSTICK_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);

	/* Start joystick reporting task */
	Scheduler_SetTaskMode(USB_Joystick_Report, TASK_RUN);
}

HANDLES_EVENT(USB_UnhandledControlPacket)
{
	/* Handle HID Class specific requests */
	switch (Request)
	{
		case REQ_GetReport:
			if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Ignore unused Report Type and Report ID values */
				Endpoint_Ignore_Word();
				
				/* Ignore unused Interface number value */
				Endpoint_Ignore_Word();

				/* Read in the number of bytes in the report to send to the host */
				uint16_t BytesToSend     = Endpoint_Read_Word_LE();
				
				/* Get a pointer to the HID report and determine its size from the HID descriptor */
				uint8_t* ReportPointer   = (uint8_t*)&JoystickReport;
				uint16_t ReportSize      = pgm_read_word(&ConfigurationDescriptor.JoystickHID.HIDReportLength);
				
				bool     SendZLP;

				Endpoint_ClearSetupReceived();
	
				/* If trying to send more bytes than exist to the host, clamp the value at the report size */
				if (BytesToSend > ReportSize)
				  BytesToSend = ReportSize;
	
				/* If the number of bytes to send will be an integer multiple of the control endpoint size,
				   a zero length packet must be sent afterwards to terminate the transfer */
				SendZLP = !(BytesToSend % USB_ControlEndpointSize);
				
				/* Loop while still bytes to send and the host hasn't aborted the transfer (via an OUT packet) */
				while (BytesToSend && (!(Endpoint_IsSetupOUTReceived())))
				{
					/* Wait until endpoint is ready for an IN packet */
					while (!(Endpoint_IsSetupINReady()));
					
					/* Write out one packet's worth of data to the endpoint, until endpoint full or all data written */
					while (BytesToSend && (Endpoint_BytesInEndpoint() < USB_ControlEndpointSize))
					{
						Endpoint_Write_Byte(pgm_read_byte(ReportPointer++));
						BytesToSend--;
					}
					
					/* Send the endpoint packet to the host */
					Endpoint_ClearSetupIN();
				}
				
				/* Check if the host aborted the transfer prematurely with an OUT packet */
				if (Endpoint_IsSetupOUTReceived())
				{
					/* Clear the OUT packet, abort any further communications for the request */
					Endpoint_ClearSetupOUT();
					return;
				}
				
				/* If a zero length packet needs to be sent, send it now */
				if (SendZLP)
				{
					while (!(Endpoint_IsSetupINReady()));
					Endpoint_ClearSetupIN();
				}

				/* Wait until host acknowledges the transfer */
				while (!(Endpoint_IsSetupOUTReceived()));
				  Endpoint_ClearSetupOUT();
			}
		
			break;
	}
}

TASK(USB_Joystick_Report)
{
	uint8_t JoyStatus_LCL = Joystick_GetStatus();

	if (JoyStatus_LCL & JOY_UP)
	  JoystickReportData.Y =  100;
	else if (JoyStatus_LCL & JOY_DOWN)
	  JoystickReportData.Y = -100;

	if (JoyStatus_LCL & JOY_RIGHT)
	  JoystickReportData.X =  100;
	else if (JoyStatus_LCL & JOY_LEFT)
	  JoystickReportData.X = -100;

	if (JoyStatus_LCL & JOY_PRESS)
	  JoystickReportData.Button  = (1 << 1);
	  
	if (HWB_GetStatus())
	  JoystickReportData.Button |= (1 << 0);

	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		/* Select the Joystick Report Endpoint */
		Endpoint_SelectEndpoint(JOYSTICK_EPNUM);

		/* Check if Joystick Endpoint Ready for Read/Write */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Write Joystick Report Data */
			Endpoint_Write_Stream_LE(&JoystickReportData, sizeof(JoystickReportData));

			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_ClearCurrentBank();
			
			/* Clear the report data afterwards */
			JoystickReportData.X      = 0;
			JoystickReportData.Y      = 0;
			JoystickReportData.Button = 0;
		}
	}
}
