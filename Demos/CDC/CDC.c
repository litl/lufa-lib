/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Communications Device Class demonstration application.
	This gives a simple reference application for implementing
	a USB to Serial converter. Sent and recieved data on the
	serial port is communicated to the USB host.
	
	Before running, you will need to install the INF file that
	is located in the CDC project directory. This will enable
	Windows to use its inbuilt CDC drivers, negating the need
	for special Windows drivers for the device.	
*/

/*
	====== INCOMPLETE, NOT CURRENTLY FUNCTIONAL ======
*/

#include "CDC.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB CDC App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
};

/* Globals: */
CDC_Line_Coding_t LineCoding = { BaudRateBPS: 9600,
                                 CharFormat:  0,
                                 ParityType:  0,
                                 DataBits:    0     };

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Bicolour_Init();
	Serial_Init(9600);
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup CDC Notification, RX and TX Endpoints */
	Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM, ENDPOINT_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, ENDPOINT_TYPE_BULK,
		                       ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, ENDPOINT_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Process CDC specific control requests */
	uint8_t* LineCodingData = (uint8_t*)&LineCoding;

	switch (Request)
	{
		case GET_LINE_CODING:
			for (uint8_t i = 0; i < sizeof(LineCoding); i++)
			  USB_Device_Write_Byte(*(LineCodingData++));	
			
			Endpoint_ClearSetupRecieved();
			Endpoint_In_Clear();
			
			while (!(Endpoint_Out_IsRecieved()));
			Endpoint_Out_Clear();
		
			break;
		case SET_LINE_CODING:
			for (uint8_t i = 0; i < sizeof(LineCoding); i++)
			  *(LineCodingData++) = USB_Device_Read_Byte();

			Endpoint_ClearSetupRecieved();
			Endpoint_Out_Clear();

			Endpoint_In_Clear();
			while (!(Endpoint_In_IsReady()));
	
			break;
		case SET_CONTROL_LINE_STATE:
			Endpoint_ClearSetupRecieved();
			
			Endpoint_In_Clear();
			while (!(Endpoint_In_IsReady()));
	
			break;
	}
}
