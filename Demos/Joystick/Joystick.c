/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Joystick demonstration application. This gives a simple reference
	application for implementing a USB Keyboard device, for USB Joysticks
	using the standard Keyboard HID profile.
	
	This device will show up as a generic joystick device, with two buttons.
	Pressing the joystick inwards is the first button, and the HWB button
	on the USBKEY board is the second.
	
	Moving the joystick on the USBKEY moves the joystick location on the
	host computer.
*/

#include "Joystick.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB Joytick App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_Joystick_Report_ID,
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_Joystick_Report_ID     , TaskName: USB_Joystick_Report     , TaskStatus: TASK_RUN  },
};

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Joystick_Init();
	Bicolour_Init();
	HWB_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup Joystick Report Endpoint */
	Endpoint_ConfigureEndpoint(JOYSTICK_EPNUM, ENDPOINT_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, JOYSTICK_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

TASK(USB_Joystick_Report)
{
	USB_JoystickReport_Data_t JoystickReportData = {Button: 0, X: 0, Y: 0};
	uint8_t                JoyStatus_LCL   = Joystick_GetStatus();

	if (JoyStatus_LCL & JOY_UP)
	  JoystickReportData.Y =  100;
	else if (JoyStatus_LCL & JOY_DOWN)
	  JoystickReportData.Y = -100;

	if (JoyStatus_LCL & JOY_RIGHT)
	  JoystickReportData.X =  100;
	else if (JoyStatus_LCL & JOY_LEFT)
	  JoystickReportData.X = -100;

	if (JoyStatus_LCL & JOY_PRESS)
	  JoystickReportData.Button |= (1 << 1);
	  
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
			USB_Device_Write_Byte(JoystickReportData.X);
			USB_Device_Write_Byte(JoystickReportData.Y);
			USB_Device_Write_Byte(JoystickReportData.Button);
			
			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_In_Clear();
		}
	}
}
