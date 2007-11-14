/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Demonstration application for a TTL magnetic stripe reader (such as the
	Omron V3B-4K) by Denver Gingerich. See http://ossguy.com/ss_usb/ for the
	demonstration project website, including construction and support details.

	This example is based on the MyUSB Keyboard demonstration application,
	written by Denver Gingerich.
*/

/*
	This demo uses a keyboard HID driver to communicate the data collected
	a TTL magnetic stripe reader to the connected computer. The raw bitstream
	obtained from the magnetic stripe reader is "typed" through the keyboard
	driver as 0's and 1's. After every card swipe, the demo will send a Return key.

	This demo relies on the keyboard demo to compile. The keyboard demo application
	files must be located at ../Keyboard.
*/

#include "Keyboard.h"
#include "Magstripe.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB Magstripe App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_Keyboard_Report_ID,
};

/* Scheduler Task List */
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
	Magstripe_Init();
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_HIGHSPEED);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();

	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;
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
	uint8_t        MagStatus_LCL       = Magstripe_GetStatus();	
	uint16_t       StripeDataLen       = 0;
	static uint8_t StripeData[DATA_LEN];

	if (!(MagStatus_LCL & MAG_CLS))
	  return;

	while (MagStatus_LCL & MAG_CLS)
	{
		do
		{
			MagStatus_LCL = Magstripe_GetStatus();
		} while ((MagStatus_LCL & MAG_CLOCK) && (MagStatus_LCL & MAG_CLS));

		if (!(MagStatus_LCL & MAG_CLS))
		  break;

		if (!(MagStatus_LCL & MAG_DATA))
		  StripeData[StripeDataLen] = 39; // 0
		else
		  StripeData[StripeDataLen] = 30; // 1

		StripeDataLen++;

		if (StripeDataLen >= DATA_LEN)
		  StripeDataLen = 0;		

		do
		{
			MagStatus_LCL = Magstripe_GetStatus();
		} while (!(MagStatus_LCL & MAG_CLOCK) && (MagStatus_LCL & MAG_CLS));
	}

	for (uint8_t i = 0; i < StripeDataLen; i++)
	{
		Keyboard_SendKeyReport(StripeData[i]);
		Keyboard_SendKeyReport(0);
	}

	Keyboard_SendKeyReport(40);
	Keyboard_SendKeyReport(0);
}

void Keyboard_SendKeyReport(uint8_t KeyCode)
{
	USB_KeyboardReport_Data_t KeyboardReportData = {Modifier: 0, KeyCode: KeyCode};

	/* Check if the USB System is connected to a Host */
	if (USB_IsConnected)
	{
		/* Select the Keyboard Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

		/* Check if Keyboard Endpoint Ready for Data */
		while (!(Endpoint_In_IsReady()));

		/* Check if Keyboard Endpoint Ready for Read/Write */
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
