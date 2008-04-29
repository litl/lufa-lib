/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Mouse host demonstration application. This gives a simple reference
	application for implementing a USB Mouse host, for USB mice using
	the standard mouse HID profile.
	
	Mouse movement and button presses are displayed on the board LEDs,
	as well as printed out the serial terminal as formatted dY, dY and
	button status information.

	Currently only single interface mice are supported.	
*/

/*
	USB Mode:           Host
	USB Class:          Human Interface Device (HID)
	USB Subclass:       Mouse
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables 
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/

#include "MouseHost.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB Mouse Host App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Mouse_Host       , TaskStatus: TASK_STOP },
};

/* Globals */
uint8_t  MouseDataEndpointNumber;
uint16_t MouseDataEndpointSize;

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);
	
	/* Hardware Initialization */
	SerialStream_Init(9600);
	LEDs_Init();
	
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
	
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();

	/* Startup message */
	puts_P(PSTR(ESC_RESET ESC_BG_WHITE ESC_INVERSE_ON ESC_ERASE_DISPLAY
	       "Mouse Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR("Device Attached.\r\n"));
	LEDs_SetAllLEDs(LEDS_NO_LEDS);

	/* Start mouse and USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
	Scheduler_SetTaskMode(USB_Mouse_Host, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	/* Stop mouse and USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_Mouse_Host, TASK_STOP);

	puts_P(PSTR("Device Unattached.\r\n"));
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_HostError)
{
	USB_ShutDown();

	puts_P(PSTR(ESC_BG_RED "Host Mode Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);

	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
	for(;;);
}

EVENT_HANDLER(USB_DeviceEnumerationFailed)
{
	puts_P(PSTR(ESC_BG_RED "Dev Enum Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);
	printf_P(PSTR(" -- In State %d\r\n"), USB_HostState);
}

TASK(USB_Mouse_Host)
{
	uint8_t ErrorCode;

	/* Block task if device not connected */
	if (!(USB_IsConnected))
		return;

	/* Switch to determine what user-application handled host state the host state machine is in */
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
					DataLength:  0,
				};

			/* Send the request, display error and wait for device detatch if request fails */
			if (USB_Host_SendControlRequest(NULL) != HOST_SENDCONTROL_Successful)
			{
				puts_P(PSTR("Control error.\r\n"));

				/* Indicate error via status LEDs */
				LEDs_SetAllLEDs(LEDS_LED1);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}
			
			USB_HostState = HOST_STATE_Configured;
			break;
		case HOST_STATE_Configured:
			puts_P(PSTR("Getting Config Data.\r\n"));
		
			/* Get and process the configuration descriptor data */
			if ((ErrorCode = GetConfigDescriptorData()) != SuccessfulConfigRead)
			{
				if (ErrorCode == ControlError)
				  puts_P(PSTR("Control Error.\r\n"));
				else
				  printf_P(PSTR("Invalid Device, Error Code %d.\r\n"), ErrorCode);
				
				/* Indicate error via status LEDs */
				LEDs_SetAllLEDs(LEDS_LED1);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}

			/* Configure the mouse data pipe */
			Pipe_ConfigurePipe(MOUSE_DATAPIPE, EP_TYPE_INTERRUPT, PIPE_TOKEN_IN,
			                   MouseDataEndpointNumber, MouseDataEndpointSize, PIPE_BANK_SINGLE);

			Pipe_SetInfiniteINRequests();
		
			puts_P(PSTR("Mouse Enumerated.\r\n"));
				
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Select and unfreeze mouse data pipe */
			Pipe_SelectPipe(MOUSE_DATAPIPE);	
			Pipe_Unfreeze();

			/* Check if data has been recieved from the attached mouse */
			if (Pipe_ReadWriteAllowed())
			{
				USB_MouseReport_Data_t MouseReport;
				uint8_t                LEDMask = LEDS_NO_LEDS;

				/* Read in mouse report data */
				MouseReport.Button = Pipe_Read_Byte();
				MouseReport.X      = Pipe_Read_Byte();
				MouseReport.Y      = Pipe_Read_Byte();
				
				/* Alter status LEDs according to mouse X movement */
				if (MouseReport.X > 0)
				  LEDMask |= LEDS_LED1;
				else if (MouseReport.X < 0)
				  LEDMask |= LEDS_LED2;
				
				/* Alter status LEDs according to mouse Y movement */
				if (MouseReport.Y > 0)
				  LEDMask |= LEDS_LED3;
				else if (MouseReport.Y < 0)
				  LEDMask |= LEDS_LED4;

				/* Alter status LEDs according to mouse button position */
				if (MouseReport.Button)
				  LEDMask = LEDS_ALL_LEDS;
				  
				LEDs_SetAllLEDs(LEDMask);
				
				/* Print mouse report data through the serial port */
				printf_P(PSTR("dX:%2d dY:%2d Button:%d\r\n"), MouseReport.X,
				                                              MouseReport.Y,
				                                              MouseReport.Button);
					
				/* Clear the IN endpoint, ready for next data packet */
				Pipe_FIFOCON_Clear();
			}

			/* Freeze mouse data pipe */
			Pipe_Freeze();
			break;
	}
}

uint8_t GetConfigDescriptorData(void)
{
	uint8_t* ConfigDescriptorData;
	uint16_t ConfigDescriptorSize;
	
	/* Get Configuration Descriptor size from the device */
	if (USB_Host_GetDeviceConfigDescriptor(&ConfigDescriptorSize, NULL) != HOST_SENDCONTROL_Successful)
	  return ControlError;
	
	/* Ensure that the Configuration Descriptor isn't too large */
	if (ConfigDescriptorSize > MAX_CONFIG_DESCRIPTOR_SIZE)
	  return DescriptorTooLarge;
	  
	/* Allocate enough memory for the entire config descriptor */
	ConfigDescriptorData = alloca(ConfigDescriptorSize);

	/* Retrieve the entire configuration descriptor into the allocated buffer */
	USB_Host_GetDeviceConfigDescriptor(&ConfigDescriptorSize, ConfigDescriptorData);
	
	/* Validate returned data - ensure first entry is a configuration header descriptor */
	if (DESCRIPTOR_TYPE(ConfigDescriptorData) != DTYPE_Configuration)
	  return ControlError;
	
	while (ConfigDescriptorSize)
	{
		/* Get the next interface descriptor from the configuration descriptor */
		USB_Host_GetNextDescriptorOfType(&ConfigDescriptorSize, &ConfigDescriptorData, DTYPE_Interface);
	
		/* If reached end of configuration descriptor, error out */
		if (ConfigDescriptorSize == 0)
		  return NoHIDInterfaceFound;

		/* Check the HID descriptor class and protocol, break out if correct class/protocol interface found */
		if ((DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).Class    == MOUSE_CLASS) &&
		    (DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).Protocol == MOUSE_PROTOCOL))
		{
			break;
		}
	}

	/* If reached end of configuration descriptor, error out */
	if (ConfigDescriptorSize == 0)
	  return NoHIDInterfaceFound;
	
	/* Find the next IN endpoint descriptor after the mouse interface descriptor */
	while (ConfigDescriptorSize)
	{
		/* Get the next endpoint descriptor from the configuration descriptor data before the next interface descriptor*/
		USB_Host_GetNextDescriptorOfTypeBefore(&ConfigDescriptorSize, &ConfigDescriptorData,
		                                       DTYPE_Endpoint, DTYPE_Interface);
	
		/* If reached end of configuration descriptor, error out */
		if (ConfigDescriptorSize == 0)
		  return NoEndpointFound;

		/* Process the endpoint descriptor if it is of the IN type */
		if (DESCRIPTOR_CAST(ConfigDescriptorData,
		                    USB_Descriptor_Endpoint_t).EndpointAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
		{
			/* Retrieve the endpoint address from the endpoint descriptor */
			MouseDataEndpointNumber = DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t).EndpointAddress;
			MouseDataEndpointSize   = DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t).EndpointSize;
			
			/* Valid data found, return success */
			return SuccessfulConfigRead;
		}
	}
	
	/* If this point reached, no valid data endpoint found, return error */
	return NoEndpointFound;
}
