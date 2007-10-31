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

TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_Mouse_Report_ID,
};

TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_Mouse_Report_ID     , TaskName: USB_Mouse_Report     , TaskStatus: TASK_RUN  },
};

USB_HID_Report_Mouse_t MouseReport PROGMEM =
{
	ReportData:
	{
      0x05,0x01,          /* Usage Page (Generic Desktop)      */
      0x09,0x02,          /* Usage (Mouse)                     */
      0xA1,0x01,          /* Collection (Application)          */
      0x09,0x01,          /* Usage (Pointer)                   */
      0xA1,0x00,          /* Collection (Application)          */
      0x05,0x09,          /* Usage Page (Button)               */
      0x19,0x01,          /* Usage Minimum (1)                 */
      0x29,0x01,          /* Usage Maximum (1)                 */
      0x15,0x00,          /* Logical Minimum (0)               */
      0x25,0x01,          /* Logical Maximum (1)               */
      0x75,0x01,          /* Report Size (1)                   */
      0x95,0x01,          /* Report Count (1)                  */
      0x81,0x02,          /* Input (Data, Variable, Absolute)  */
      0x75,0x07,          /* Report Size (7)                   */
      0x95,0x01,          /* Report Count (1)                  */
      0x81,0x01,          /* Input (Constant)                  */
      0x05,0x01,          /* Usage Page (Generic Desktop Control)*/
      0x09,0x30,          /* Usage X                           */
      0x09,0x31,          /* Usage Y                           */
      0x09,0x38,          /* Usage Wheel                       */
      0x15,0x81,          /* Logical Minimum (-127)            */
      0x25,0x7F,          /* Logical Maximum (127)             */
      0x75,0x08,          /* Report Size (8)                   */
      0x95,0x02,          /* Report Count (2)                  */
      0x81,0x06,          /* Input (Data, Variable, Relative)  */
      0xC0,               /* End Collection                    */
      0xC0                /* End Collection                    */
	}
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

	/* Scheduling */
	Scheduler_Start(); // Scheduler never returns, so put this last
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	Endpoint_ConfigureEndpoint(MOUSE_EPNUM, ENDPOINT_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, MOUSE_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

TASK(USB_Mouse_Report)
{
	USB_MouseReport_Data_t MouseReportData = {Button: 0, X: 0, Y: 0};
	uint8_t                JoyStatus_LCL   = Joystick_GetStatus();

	if (JoyStatus_LCL & JOY_UP)
	  MouseReportData.Y++;
	else if (JoyStatus_LCL & JOY_DOWN)
	  MouseReportData.Y--;

	if (JoyStatus_LCL & JOY_RIGHT)
	  MouseReportData.X++;
	else if (JoyStatus_LCL & JOY_LEFT)
	  MouseReportData.X--;

	if (JoyStatus_LCL & JOY_PRESS)
	  MouseReportData.Button = 0x01;

	if (USB_IsConnected && USB_IsInitialized)
	{
		Endpoint_SelectEndpoint(MOUSE_EPNUM);

		if (Endpoint_ReadWriteAllowed())
		{
			USB_Device_Write_Byte(MouseReportData.Button);
			USB_Device_Write_Byte(MouseReportData.X);
			USB_Device_Write_Byte(MouseReportData.Y);
			
			USB_In_Clear();
		}
	}
}

