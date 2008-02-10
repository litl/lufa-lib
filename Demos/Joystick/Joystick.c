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
	on the USBKEY board is the second.
	
	Moving the joystick on the USBKEY moves the joystick location on the
	host computer.
	
	Currently only single interface joysticks are supported.
*/

#include "Joystick.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB Joystick App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Joystick_Report  , TaskStatus: TASK_STOP },
};

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	wdt_disable();

	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Joystick_Init();
	Bicolour_Init();
	HWB_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

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
	/* Stop running joystick reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_Joystick_Report, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup Joystick Report Endpoint */
	Endpoint_ConfigureEndpoint(JOYSTICK_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, JOYSTICK_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);

	/* Start joystick reporting task */
	Scheduler_SetTaskMode(USB_Joystick_Report, TASK_RUN);
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
			Endpoint_Write_Byte(JoystickReportData.X);
			Endpoint_Write_Byte(JoystickReportData.Y);
			Endpoint_Write_Byte(JoystickReportData.Button);
			
			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_FIFOCON_Clear();
		}
	}
}
