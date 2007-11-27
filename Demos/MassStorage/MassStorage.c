/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Mass Storage demonstration application. This gives a simple reference
	application for implementing a USB Mass Storage device using the basic
	USB UFI drivers in all modern OSes (i.e. no special drivers required).
	
	On startup the system will automatically enumerate and function as an
	external mass storage device which may be formatted and used in the
	same manner as commercial USB Mass Storage devices.
*/

/*
		---=== UNFINISHED AND NON-OPERATIONAL ===---
*/

#include "MassStorage.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB MassStore App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_MassStorage_ID,
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_MassStorage_ID      , TaskName: USB_MassStorage      , TaskStatus: TASK_RUN  },
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
	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup Mass Storage In and Out Endpoints */
	Endpoint_ConfigureEndpoint(MASS_STORAGE_IN_EPNUM, ENDPOINT_TYPE_BULK,
		                       ENDPOINT_DIR_IN, MASS_STORAGE_IO_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	Endpoint_ConfigureEndpoint(MASS_STORAGE_OUT_EPNUM, ENDPOINT_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, MASS_STORAGE_IO_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Process UFI specific control requests */

}
	
TASK(USB_MassStorage)
{
	if (USB_IsConnected)
	{
		/* Select the Data Out Endpoint */
		Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPNUM);
		
		if (Endpoint_Out_IsRecieved())
		{	
			/* Process command block */

			/* Return command status */

			Endpoint_Out_Clear();
		}
	}
}
