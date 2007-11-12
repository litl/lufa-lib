/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
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
	A, B, C, D and E. See the USB HID documentation for more information
	on sending keyboard event and keypresses.
*/

#include "Keyboard.h"

TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_Keyboard_Report_ID,
};

TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_Keyboard_Report_ID  , TaskName: USB_Keyboard_Report  , TaskStatus: TASK_RUN  },
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
	/* Setup Keyboard Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_EPNUM, ENDPOINT_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
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

		/* Check if Keyboard Endpoint Ready for Data */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Write Keyboard Report Data */
			USB_Device_Write_Byte(KeyboardReportData.Modifier);
			USB_Device_Write_Byte(KeyboardReportData.KeyCode);
			
			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_In_Clear();
		}
	}
}

