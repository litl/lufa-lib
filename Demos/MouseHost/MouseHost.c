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
	
	Currently only single-interface mice are supported.
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
	SerialStream_Init(9600);
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_HOST, USB_HOST_OPT_AUTOVBUS | USB_OPT_REG_ENABLED);

	/* Startup message */
	puts_P(PSTR(ESC_RESET ESC_BG_WHITE ESC_INVERSE_ON ESC_ERASE_DISPLAY
	       "Mouse Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR("Device Attached.\r\n"));
	Bicolour_SetLeds(BICOLOUR_NO_LEDS);	
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	puts_P(PSTR("Device Unattached.\r\n"));
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
}

EVENT_HANDLER(USB_HostError)
{
	USB_ShutDown();

	puts_P(PSTR(ESC_BG_RED "Host Mode Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);

	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	for(;;);
}

TASK(USB_Mouse_Host)
{
	static uint8_t DataBuffer[sizeof(USB_Descriptor_Configuration_Header_t) +
				              sizeof(USB_Descriptor_Interface_t)];

	/* Block task if device not connected */
	if (!(USB_IsConnected))
		return;

	switch (USB_HostState)
	{
		case HOST_STATE_Addressed:
			USB_HostRequest.RequestType    = (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE);
			USB_HostRequest.RequestData    = REQ_SetConfiguration;
			USB_HostRequest.Value          = 1;
			USB_HostRequest.Index          = 0;
			USB_HostRequest.Length         = USB_ControlPipeSize;

			if (USB_Host_SendControlRequest(NULL) != HOST_SENDCONTROL_Sucessful)
			{
				puts_P(PSTR("Control error."));

				Bicolour_SetLeds(BICOLOUR_LED1_RED);
				while (USB_IsConnected); // Wait for device detatch
				break;
			}
			
			Pipe_ConfigurePipe(MOUSE_DATAPIPE, PIPE_TYPE_INTERRUPT, PIPE_TOKEN_IN, 1, 8, PIPE_BANK_SINGLE);
			Pipe_SelectPipe(MOUSE_DATAPIPE);
			Pipe_SetInfiniteINRequests();
		
			USB_HostState = HOST_STATE_Configured;
			break;
		case HOST_STATE_Configured:
			USB_HostRequest.RequestType    = (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE);
			USB_HostRequest.RequestData    = REQ_GetDescriptor;
			USB_HostRequest.Value_HighByte = DTYPE_Configuration;
			USB_HostRequest.Value_LowByte  = 0;
			USB_HostRequest.Index          = 0;
			USB_HostRequest.Length         = sizeof(DataBuffer);

			if (USB_Host_SendControlRequest(DataBuffer)
			    != HOST_SENDCONTROL_Sucessful)
			{
				puts_P(PSTR("Control error."));
			
				Bicolour_SetLeds(BICOLOUR_LED1_RED);
				while (USB_IsConnected); // Wait for device detatch
				break;
			}

			if (DataBuffer[sizeof(USB_Descriptor_Configuration_Header_t) +
			               offsetof(USB_Descriptor_Interface_t, Class)] != MOUSE_CLASS)
			{
				puts_P(PSTR("Incorrect device class."));

				Bicolour_SetLeds(BICOLOUR_LED1_RED);
				while (USB_IsConnected); // Wait for device detatch
				break;
			}
				
			if (DataBuffer[sizeof(USB_Descriptor_Configuration_Header_t) +
			               offsetof(USB_Descriptor_Interface_t, Protocol)] != MOUSE_PROTOCOL)
			{
				puts_P(PSTR("Incorrect device protocol."));

				Bicolour_SetLeds(BICOLOUR_LED1_RED);
				while (USB_IsConnected); // Wait for device detatch
				break;
			}

			puts_P(PSTR("Mouse Enumerated.\r\n"));
				
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			Pipe_SelectPipe(MOUSE_DATAPIPE);	
			Pipe_Unfreeze();

			if (Pipe_In_IsRecieved())
			{
				USB_MouseReport_Data_t MouseReport;
					
				MouseReport.Button = USB_Host_Read_Byte();
				MouseReport.X      = USB_Host_Read_Byte();
				MouseReport.Y      = USB_Host_Read_Byte();
					
				Bicolour_SetLeds(BICOLOUR_NO_LEDS);
					
				if (MouseReport.X > 0)
				  Bicolour_SetLed(BICOLOUR_LED1, BICOLOUR_LED1_GREEN);
				else if (MouseReport.X < 0)
				  Bicolour_SetLed(BICOLOUR_LED1, BICOLOUR_LED1_RED);						
				
				if (MouseReport.Y > 0)
				  Bicolour_SetLed(BICOLOUR_LED2, BICOLOUR_LED2_GREEN);
				else if (MouseReport.Y < 0)
				  Bicolour_SetLed(BICOLOUR_LED2, BICOLOUR_LED2_RED);						

				if (MouseReport.Button)
				  Bicolour_SetLeds(BICOLOUR_ALL_LEDS);
					
				Pipe_In_Clear();
				Pipe_ResetFIFO();
			}

			Pipe_Freeze();
			break;
	}
}
