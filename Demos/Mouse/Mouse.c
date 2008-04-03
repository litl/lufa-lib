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

EVENT_HANDLER(USB_CreateEndpoints)
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

TASK(USB_Mouse_Report)
{
	USB_MouseReport_Data_t MouseReportData = {Button: 0, X: 0, Y: 0};
	uint8_t                JoyStatus_LCL   = Joystick_GetStatus();

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
			Endpoint_Write_Byte(MouseReportData.Button);
			Endpoint_Write_Byte(MouseReportData.X);
			Endpoint_Write_Byte(MouseReportData.Y);
			
			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_FIFOCON_Clear();
		}
	}
}
