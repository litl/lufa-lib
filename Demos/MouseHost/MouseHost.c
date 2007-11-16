/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Mouse host demonstration application. This gives a simple reference
	application for implementing a USB Mouse host, for USB mice using
	the standard mouse HID profile.	
*/

/*
	========= INCOMPLETE AND NOT CURRENTLY FUNCTIONING =========
*/

#include "MouseHost.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB Mouse Host App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_Mouse_Host_ID,
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_Mouse_Host_ID       , TaskName: USB_Mouse_Host       , TaskStatus: TASK_RUN  },
};

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_HOST, USB_HOST_AUTOVBUS);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	Bicolour_SetLeds(BICOLOUR_NO_LEDS);	
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
}
		
TASK(USB_Mouse_Host)
{
	if (USB_IsConnected)
	{
		switch (USB_HostState)
		{
			case HOST_STATE_Addressed:
				// Get device class, compare against mouse HID and switch to HOST_STATE_Configured if valid
				break;
			case HOST_STATE_Configured:
				// Get mouse endpoint data here, set bicolour LEDs
				break;
		}
	}
}
