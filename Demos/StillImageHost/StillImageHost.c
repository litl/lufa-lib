/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Still Image host demonstration application. This gives a simple reference
	application for implementing a Still Image host, for USB devices such as
	digital cameras.
	
	*** WORK IN PROGRESS - NOT CURRENTLY FUNCTIONING ***
*/

#include "StillImageHost.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB SIMG Host App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_SImage_Host      , TaskStatus: TASK_STOP },
};

/* Globals */
uint8_t  SImageEndpointNumber_IN;
uint8_t  SImageEndpointNumber_OUT;
uint8_t  SImageEndpointNumber_EVENTS;
uint16_t SImageEndpointSize_IN;
uint16_t SImageEndpointSize_OUT;
uint16_t SImageEndpointSize_EVENTS;
uint8_t  SImageEndpointIntFreq_EVENTS;

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

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
	       "Still Image Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR("Device Attached.\r\n"));
	LEDs_SetAllLEDs(LEDS_NO_LEDS);
	
	/* Start USB management and Still Image tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
	Scheduler_SetTaskMode(USB_SImage_Host, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	/* Stop USB management and Still Image tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_SImage_Host, TASK_STOP);

	puts_P(PSTR("\r\nDevice Unattached.\r\n"));
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

TASK(USB_SImage_Host)
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
				  puts_P(PSTR("Control Error (Get Configuration).\r\n"));
				else
				  puts_P(PSTR("Invalid Device.\r\n"));

				printf_P(PSTR(" -- Error Code: %d\r\n"), ErrorCode);
				
				/* Indicate error via status LEDs */
				LEDs_SetAllLEDs(LEDS_LED1);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}

			Pipe_ConfigurePipe(SIMAGE_DATA_IN_PIPE, EP_TYPE_BULK, PIPE_TOKEN_IN,
			                   SImageEndpointNumber_IN, SImageEndpointSize_IN,
			                   PIPE_BANK_DOUBLE);			
		
			Pipe_ConfigurePipe(SIMAGE_DATA_OUT_PIPE, EP_TYPE_BULK, PIPE_TOKEN_OUT,
			                   SImageEndpointNumber_OUT, SImageEndpointSize_OUT,
			                   PIPE_BANK_DOUBLE);			

			Pipe_ConfigurePipe(SIMAGE_EVENTS_PIPE, EP_TYPE_INTERRUPT, PIPE_TOKEN_IN,
			                   SImageEndpointNumber_EVENTS, SImageEndpointSize_EVENTS,
			                   PIPE_BANK_DOUBLE);			

			Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);
			Pipe_SetInfiniteINRequests();
			Pipe_Freeze();
			
			Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);
			Pipe_Freeze();
			
			Pipe_SelectPipe(SIMAGE_EVENTS_PIPE);
			Pipe_SetInfiniteINRequests();
			Pipe_SetInterruptFreq(SImageEndpointIntFreq_EVENTS);

			puts_P(PSTR("Still Image Device Enumerated.\r\n"));
				
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Indicate device busy via the status LEDs */
			LEDs_SetAllLEDs(LEDS_LED3 | LEDS_LED4);

			uint8_t ReturnCode;
			
			puts_P(PSTR("Retrieving Device Info...\r\n"));
			
			if ((ReturnCode = SImage_GetInfo()) != NoError)
			{
				ShowCommandError(ReturnCode);
				break;
			}
			
			puts_P(PSTR("Opening Session...\r\n"));

			if ((ReturnCode = SImage_OpenCloseSession(0x0001, SESSION_OPEN)) != NoError)
			{
				ShowCommandError(ReturnCode);
				break;
			}

			puts_P(PSTR("Retrieving Objects...\r\n"));

			puts_P(PSTR("Closing Session...\r\n"));

			if ((ReturnCode = SImage_OpenCloseSession(0x0001, SESSION_CLOSE)) != NoError)
			{
				ShowCommandError(ReturnCode);
				break;
			}

			puts_P(PSTR("Done.\r\n"));

			/* Indicate device no longer busy */
			LEDs_SetAllLEDs(LEDS_LED4);
			
			/* Wait until USB device disconnected */
			while (USB_IsConnected);
			
			break;
	}
}

void ShowCommandError(uint8_t ErrorCode)
{
	printf_P(PSTR("Device failed command %d: %d.\r\n"), PIMA_Command.Code, ErrorCode);
			
	/* Indicate error via status LEDs */
	LEDs_SetAllLEDs(LEDS_LED1);
				
	/* Wait until USB device disconnected */
	while (USB_IsConnected);
}

uint8_t GetConfigDescriptorData(void)
{
	uint8_t* ConfigDescriptorData;
	uint16_t ConfigDescriptorSize;
	uint8_t  ErrorCode;
	
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
	
	/* Get the Still Image interface from the configuration descriptor */
	if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
	                                                NextStillImageInterface)))
	{
		/* Descriptor not found, error out */
		return NoInterfaceFound;
	}

	/* Get the IN and OUT data and event endpoints for the Still Image interface */
	while (!(SImageEndpointNumber_IN && SImageEndpointNumber_OUT && SImageEndpointNumber_EVENTS))
	{
		/* Fetch the next endpoint from the current Still Image interface */
		if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
		                                                NextSImageInterfaceDataEndpoint)))
		{
			/* Descriptor not found, error out */
			return NoEndpointFound;
		}
		
		USB_Descriptor_Endpoint_t* EndpointData = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t);

		/* Check if the found endpoint is a interrupt or bulk type descriptor */
		if ((EndpointData->Attributes & EP_TYPE_MASK) == EP_TYPE_INTERRUPT)
		{
			/* If the endpoint is a IN type interrupt endpoint, set appropriate globals */
			if (EndpointData->EndpointAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
			{
				SImageEndpointNumber_EVENTS  = EndpointData->EndpointAddress;
				SImageEndpointSize_EVENTS    = EndpointData->EndpointSize;
				SImageEndpointIntFreq_EVENTS = EndpointData->PollingIntervalMS;
			}
		}
		else
		{
			/* Check if the endpoint is a bulk IN or bulk OUT endpoint, set appropriate globals */
			if (EndpointData->EndpointAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
			{
				SImageEndpointNumber_IN  = EndpointData->EndpointAddress;
				SImageEndpointSize_IN    = EndpointData->EndpointSize;
			}
			else
			{
				SImageEndpointNumber_OUT = EndpointData->EndpointAddress;
				SImageEndpointSize_OUT   = EndpointData->EndpointSize;
			}
		}
	}

	/* Valid data found, return success */
	return SuccessfulConfigRead;
}

DESCRIPTOR_COMPARATOR(NextStillImageInterface)
{
	/* Descriptor Search Comparitor Function - find next Still Image class interface descriptor */

	if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Interface)
	{
		/* Check the descriptor class and protocol, break out if correct class/protocol interface found */
		if ((DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).Class    == SIMAGE_CLASS)    &&
		    (DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).SubClass == SIMAGE_SUBCLASS) &&
		    (DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).Protocol == SIMAGE_PROTOCOL))
		{
			return Descriptor_Search_Found;
		}
	}
	
	return Descriptor_Search_NotFound;
}

DESCRIPTOR_COMPARATOR(NextSImageInterfaceDataEndpoint)
{
	/* Descriptor Search Comparitor Function - find next interface BULK or INTERRUPT endpoint descriptor before
	                                           next interface descriptor */

	if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Endpoint)
	{
		uint8_t EndpointType = (DESCRIPTOR_CAST(CurrentDescriptor,
		                                        USB_Descriptor_Endpoint_t).Attributes & EP_TYPE_MASK);
	
		if ((EndpointType == EP_TYPE_BULK) || (EndpointType == EP_TYPE_INTERRUPT))
		  return Descriptor_Search_Found;
	}
	else if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Interface)
	{
		return Descriptor_Search_Fail;
	}

	return Descriptor_Search_NotFound;
}
