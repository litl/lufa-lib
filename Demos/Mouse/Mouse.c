/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Mouse demonstration application. This gives a simple reference
	application for implementing a USB Mouse using the basic USB HID
	drivers in all modern OSes (i.e. no special drivers required).
	
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

#include "Mouse.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB Mouse App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Mouse_Report     , TaskStatus: TASK_STOP },
};

/* Global Variables */
USB_MouseReport_Data_t MouseReportData = {Button: 0, X: 0, Y: 0};


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
	/* Stop running mouse reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_Mouse_Report, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Mouse Report Endpoint */
	Endpoint_ConfigureEndpoint(MOUSE_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, MOUSE_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);

	/* Start running mouse reporting task */
	Scheduler_SetTaskMode(USB_Mouse_Report, TASK_RUN);
}

HANDLES_EVENT(USB_UnhandledControlPacket)
{
	/* Handle HID Class specific requests */
	switch (Request)
	{
		case REQ_GetReport:
			if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Ignore report type and ID number value */
				Endpoint_Ignore_Word();
				
				/* Ignore unused Interface number value */
				Endpoint_Ignore_Word();

				/* Read in the number of bytes in the report to send to the host */
				uint16_t BytesToSend     = Endpoint_Read_Word_LE();
				
				/* Get a pointer to the HID IN report */
				uint8_t* ReportPointer   = (uint8_t*)&MouseReportData;
				uint16_t ReportSize      = sizeof(MouseReportData);
				
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

TASK(USB_Mouse_Report)
{
	uint8_t JoyStatus_LCL = Joystick_GetStatus();

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
	  
	if (HWB_GetStatus())
	  MouseReportData.Button |= (1 << 1);

	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		/* Select the Mouse Report Endpoint */
		Endpoint_SelectEndpoint(MOUSE_EPNUM);

		/* Check if Mouse Endpoint Ready for Read/Write */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Write Mouse Report Data */
			Endpoint_Write_Stream_LE(&MouseReportData, sizeof(MouseReportData));
			
			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_ClearCurrentBank();
			
			/* Clear the report data afterwards */
			MouseReportData.Button = 0;
			MouseReportData.X = 0;			
			MouseReportData.Y = 0;
		}
	}
}
