/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Keyboard host demonstration application. This gives a simple reference
	application for implementing a USB Keyboard host, for USB keyboards using
	the standard Keyboard HID profile.
	
	Pressed alpha-numeric. enter or space key is transmitted through the serial
	USART at serial settings 9600, 8, N, 1.
*/

/*
	========= INCOMPLETE =========
*/

#include "KeyboardHost.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB Keyboard Host App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_Keyboard_Host_ID,
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_Keyboard_Host_ID    , TaskName: USB_Keyboard_Host    , TaskStatus: TASK_RUN  },
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
	       "MyUSB Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
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
		
TASK(USB_Keyboard_Host)
{
	if (USB_IsConnected)
	{
		switch (USB_HostState)
		{
			case HOST_STATE_Addressed:
/*
				USB_HostRequest.RequestType    = (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE);
				USB_HostRequest.RequestData    = REQ_GetDescriptor;
				USB_HostRequest.Value_HighByte = DTYPE_Configuration;
				USB_HostRequest.Value_LowByte  = 0;
				USB_HostRequest.Index          = 0;
				USB_HostRequest.Length         = USB_ControlPipeSize;

				if (USB_Host_SendControlRequest() == SEND_CONTROL_ERROR)
				{
					Bicolour_SetLeds(BICOLOUR_LED1_RED);
					JTAG_DEBUG_BREAK(); // TEMP
					while (USB_IsConnected); // Wait for device detatch
					break;
				}

				Pipe_In_Clear();
*/				

				// Check to ensure connected device is a Keyboard here

				USB_HostState = HOST_STATE_Configured;
				break;
			case HOST_STATE_Configured:
				USB_HostRequest.RequestType    = (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE);
				USB_HostRequest.RequestData    = REQ_SetConfiguration;
				USB_HostRequest.Value          = 1;
				USB_HostRequest.Index          = 0;
				USB_HostRequest.Length         = USB_ControlPipeSize;

				if (USB_Host_SendControlRequest(NULL, 0) == HOST_SEND_CONTROL_ERROR)
				{
					Bicolour_SetLeds(BICOLOUR_LED1_RED);
					while (USB_IsConnected); // Wait for device detatch
					break;
				}
				
				Pipe_ConfigurePipe(1, PIPE_TYPE_INTERRUPT, PIPE_TOKEN_IN, 1, 8, PIPE_BANK_SINGLE);
				Pipe_SelectPipe(1);
				Pipe_SetInfiniteINRequests();		

				USB_HostState = HOST_STATE_Ready;
				break;
			case HOST_STATE_Ready:
				Pipe_SelectPipe(1);	
				Pipe_Unfreeze();

				if (Pipe_In_IsRecieved())
				{
					USB_KeyboardReport_Data_t KeyboardReport;
					
					KeyboardReport.Modifier = USB_Host_Read_Byte();
					USB_Host_Ignore_Byte();
					KeyboardReport.KeyCode  = USB_Host_Read_Byte();
					
					Bicolour_SetLed(BICOLOUR_LED1, (KeyboardReport.Modifier) ? BICOLOUR_LED1_RED
					                                                         : BICOLOUR_LED1_OFF);
					
					if (KeyboardReport.KeyCode)
					{
						if (Bicolour_GetLeds() & BICOLOUR_LED2_GREEN)
						  Bicolour_TurnOffLeds(BICOLOUR_LED2_GREEN);
						else
						  Bicolour_TurnOnLeds(BICOLOUR_LED2_GREEN);
						  
						char PressedKey = 0;

						if ((KeyboardReport.KeyCode >= 0x04) &&
						    (KeyboardReport.KeyCode <= 0x1D))
						{
							PressedKey = (KeyboardReport.KeyCode - 0x04) + 'A';
						}
						else if ((KeyboardReport.KeyCode >= 0x1E) &&
						         (KeyboardReport.KeyCode <= 0x27))
						{
							PressedKey = (KeyboardReport.KeyCode - 0x1E) + '0';
						}
						else if (KeyboardReport.KeyCode == 0x2C)
						{
							PressedKey = ' ';						
						}
						else if (KeyboardReport.KeyCode == 0x28)
						{
							PressedKey = '\n';
						}
						 
						if (PressedKey)
						  printf_P(PSTR("%c"), PressedKey);
					}
					
					Pipe_In_Clear();
					Pipe_ResetFIFO();
				}

				Pipe_Freeze();

				break;
		}
	}
}