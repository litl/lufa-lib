/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Mouse demonstration application. This gives a simple reference
	application for implementing a USB Mouse using the basic USB HID
	drivers in all modern OSes (i.e. no special drivers required).
	
	On startup the system will automatically enumerate and function
	as a mouse when the USB connection to a host is present. To use
	the mouse, move the joystick to move the pointer, and push the
	joystick inwards to simulate a left-button click.
*/

#include "Mouse.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB Mouse App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_Mouse_Report_ID,
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_Mouse_Report_ID     , TaskName: USB_Mouse_Report     , TaskStatus: TASK_RUN  },
};

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Joystick_Init();
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_HIGHSPEED);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup Mouse Report Endpoint */
	Endpoint_ConfigureEndpoint(MOUSE_EPNUM, ENDPOINT_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, MOUSE_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

TASK(USB_Mouse_Report)
{
	USB_MouseReport_Data_t MouseReportData = {Button: false, X: 0, Y: 0};
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
	  MouseReportData.Button = true;

	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		/* Select the Mouse Report Endpoint */
		Endpoint_SelectEndpoint(MOUSE_EPNUM);

		/* Check if Mouse Endpoint Ready for Data */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Write Mouse Report Data */
			USB_Device_Write_Byte(MouseReportData.Button);
			USB_Device_Write_Byte(MouseReportData.X);
			USB_Device_Write_Byte(MouseReportData.Y);
			
			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_In_Clear();
		}
	}
}

