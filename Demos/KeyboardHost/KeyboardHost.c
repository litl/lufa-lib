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
	
	Pressed alpha-numeric, enter or space key is transmitted through the serial
	USART at serial settings 9600, 8, N, 1.

	Currently only single-interface keyboards are supported.
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

/* Globals */
uint8_t KeyboardDataEndpointNumber;

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
	USB_Init(USB_MODE_HOST, USB_OPT_REG_ENABLED);

	/* Startup message */
	puts_P(PSTR(ESC_RESET ESC_BG_WHITE ESC_INVERSE_ON ESC_ERASE_DISPLAY
	       "Keyboard Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
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
	puts_P(PSTR("\r\nDevice Unattached.\r\n"));
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

TASK(USB_Keyboard_Host)
{
	uint8_t ErrorCode;

	/* Block task if device not connected */
	if (!(USB_IsConnected))
		return;

	switch (USB_HostState)
	{
		case HOST_STATE_Addressed:
			/* Standard request to set the device configuration to configuration 1 */
			USB_HostRequest = (USB_Host_Request_Header_t)
				{
					RequestType: (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE),
					RequestData: REQ_SetConfiguration,
					Value:       1,
					Index:       0,
					Length:      USB_ControlPipeSize,
				};

			/* Send the request, display error and wait for device detatch if request fails */
			if (USB_Host_SendControlRequest(NULL) != HOST_SENDCONTROL_Sucessful)
			{
				puts_P(PSTR("Control error."));

				Bicolour_SetLeds(BICOLOUR_LED1_RED);
				while (USB_IsConnected);
				break;
			}
				
			USB_HostState = HOST_STATE_Configured;
			break;
		case HOST_STATE_Configured:
			puts_P(PSTR("Getting Config Data.\r\n"));
		
			/* Get and process the configuration descriptor data */
			ErrorCode = GetConfigDescriptorData();
			
			/* Check if the configuration descriptor processing was sucessful */
			if (ErrorCode != SuccessfulConfigRead)
			{
				switch (ErrorCode)
				{
					case HIDInterfaceNotFound:
						puts_P(PSTR("Invalid Device Type.\r\n"));
						break;
					case IncorrectProtocol:
						puts_P(PSTR("Invalid Protocol.\r\n"));
						break;
					default:
						puts_P(PSTR("Control Error.\r\n"));
						break;
				}

				/* Indicate error via status LEDs */
				Bicolour_SetLeds(BICOLOUR_LED1_RED);
				
				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}

			/* Configure the keyboard data pipe */
			Pipe_ConfigurePipe(KEYBOARD_DATAPIPE, PIPE_TYPE_INTERRUPT, PIPE_TOKEN_IN,
			                   KeyboardDataEndpointNumber, 8, PIPE_BANK_SINGLE);

			Pipe_SelectPipe(KEYBOARD_DATAPIPE);
			Pipe_SetInfiniteINRequests();
		
			puts_P(PSTR("Keyboard Enumerated.\r\n"));
				
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Select and unfreeze keyboard data pipe */
			Pipe_SelectPipe(KEYBOARD_DATAPIPE);	
			Pipe_Unfreeze();

			/* Check if data has been recieved from the attached keyboard */
			if (Pipe_In_IsReceived())
			{
				USB_KeyboardReport_Data_t KeyboardReport;
					
				/* Read in keyboard report data */
				KeyboardReport.Modifier = Pipe_Read_Byte();
				Pipe_Ignore_Byte();
				KeyboardReport.KeyCode  = Pipe_Read_Byte();
					
				Bicolour_SetLed(BICOLOUR_LED1, (KeyboardReport.Modifier) ? BICOLOUR_LED1_RED
				                                                         : BICOLOUR_LED1_OFF);
				
				/* Check if a key has been pressed */
				if (KeyboardReport.KeyCode)
				{
					/* Toggle status LED to indicate keypress */
					if (Bicolour_GetLeds() & BICOLOUR_LED2_GREEN)
					  Bicolour_TurnOffLeds(BICOLOUR_LED2_GREEN);
					else
					  Bicolour_TurnOnLeds(BICOLOUR_LED2_GREEN);
						  
					char PressedKey = 0;

					/* Retrieve pressed key character if alphanumeric */
					if ((KeyboardReport.KeyCode >= 0x04) && (KeyboardReport.KeyCode <= 0x1D))
					  PressedKey = (KeyboardReport.KeyCode - 0x04) + 'A';
					else if ((KeyboardReport.KeyCode >= 0x1E) && (KeyboardReport.KeyCode <= 0x27))
					  PressedKey = (KeyboardReport.KeyCode - 0x1E) + '0';
					else if (KeyboardReport.KeyCode == 0x2C)
					  PressedKey = ' ';						
					else if (KeyboardReport.KeyCode == 0x28)
					  PressedKey = '\n';
						 
					/* Print the pressed key character out through the serial port if valid */
					if (PressedKey)
					  printf_P(PSTR("%c"), PressedKey);
				}
				
				/* Clear the IN endpoint, ready for next data packet */
				Pipe_In_Clear();
			}

			/* Freeze keyboard data pipe */
			Pipe_Freeze();
			break;
	}
}

uint8_t GetConfigDescriptorData(void)
{
	uint8_t* ConfigDescriptorData;
	uint16_t ConfigDescriptorSize;
	bool     FoundHIDInterfaceDescriptor = false;
	
	/* Get Configuration Descriptor size from the device */
	if (AVR_HOST_GetDeviceConfigDescriptorSize(&ConfigDescriptorSize) != HOST_SENDCONTROL_Sucessful)
	  return ControlError;
	
	/* Ensure that the Configuration Descriptor isn't too large */
	if (ConfigDescriptorSize > MAX_CONFIG_DESCRIPTOR_SIZE)
	  return DescriptorTooLarge;
	  
	/* Allocate enough memory for the entire config descriptor */
	ConfigDescriptorData = __builtin_alloca(ConfigDescriptorSize);

	/* Retrieve the entire configuration descriptor into the allocated buffer */
	AVR_HOST_GetDeviceConfigDescriptor(ConfigDescriptorSize, ConfigDescriptorData);
	
	/* Validate returned data - ensure first entry is a configuration header descriptor */
	if (((USB_Descriptor_Header_t*)ConfigDescriptorData)->Type != DTYPE_Configuration)
	  return ControlError;
	
	while (!(FoundHIDInterfaceDescriptor))
	{
		/* Get the next descriptor from the configuration descriptor data */
		AVR_HOST_GetNextDescriptor(&ConfigDescriptorSize, &ConfigDescriptorData);	  

		/* Find next interface descriptor */
		while (ConfigDescriptorSize)
		{
			/* Check to see if the next descriptor is an interface descriptor, if so break out */
			if (((USB_Descriptor_Header_t*)ConfigDescriptorData)->Type == DTYPE_Interface)
			  break;

			/* Get the next descriptor from the configuration descriptor data */
			AVR_HOST_GetNextDescriptor(&ConfigDescriptorSize, &ConfigDescriptorData);
		}

		/* If reached end of configuration descriptor, error out */
		if (ConfigDescriptorSize == 0)
		  return HIDInterfaceNotFound;

		/* Check the HID descriptor class, set the flag if class matches expected class */
		if (((USB_Descriptor_Interface_t*)ConfigDescriptorData)->Class == KEYBOARD_CLASS)
		  FoundHIDInterfaceDescriptor = true;
	}

	/* Check protocol - error out if it is incorrect */
	if (((USB_Descriptor_Interface_t*)ConfigDescriptorData)->Protocol != KEYBOARD_PROTOCOL)
	  return IncorrectProtocol;
	
	/* Find the next IN endpoint descriptor after the keyboard interface descriptor */
	while (ConfigDescriptorSize)
	{
		/* Check if current descritor is an IN endpoint descriptor */
		if ((((USB_Descriptor_Header_t*)ConfigDescriptorData)->Type == DTYPE_Endpoint) &&
		   (((USB_Descriptor_Endpoint_t*)ConfigDescriptorData)->EndpointAddress & ENDPOINT_DESCRIPTOR_DIR_IN))
		{
			  break;
		}
		
		/* Get the next descriptor from the configuration descriptor data */
		AVR_HOST_GetNextDescriptor(&ConfigDescriptorSize, &ConfigDescriptorData);	  		
	}
	
	/* If reached end of configuration descriptor, error out */
	if (ConfigDescriptorSize == 0)
	  return NoEndpointFound;

	/* Retrieve the endpoint address from the endpoint descriptor */
	KeyboardDataEndpointNumber = ((USB_Descriptor_Endpoint_t*)ConfigDescriptorData)->EndpointAddress;
	
	/* Valid data found, return success */
	return SuccessfulConfigRead;
}
