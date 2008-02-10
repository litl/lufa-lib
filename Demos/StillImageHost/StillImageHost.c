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

PIMA_Container_t PIMA_Command;
PIMA_Container_t PIMA_Response;
uint32_t         PIMA_TransactionID = 1;

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	wdt_disable();

	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	SerialStream_Init(9600);
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_HOST, USB_OPT_REG_ENABLED);

	/* Startup message */
	puts_P(PSTR(ESC_RESET ESC_BG_WHITE ESC_INVERSE_ON ESC_ERASE_DISPLAY
	       "Still Image Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR("Device Attached.\r\n"));
	Bicolour_SetLeds(BICOLOUR_NO_LEDS);	

	/* Start keyboard and USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
	Scheduler_SetTaskMode(USB_SImage_Host, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	/* Stop keyboard and USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_SImage_Host, TASK_STOP);

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
			if (USB_Host_SendControlRequest(NULL) != HOST_SENDCONTROL_Sucessful)
			{
				puts_P(PSTR("Control error.\r\n"));

				/* Indicate error via status LEDs */
				Bicolour_SetLeds(BICOLOUR_LED1_RED);

				/* Wait until USB device disconnected */
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
					case InterfaceNotFound:
						puts_P(PSTR("Invalid Device Type.\r\n"));
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
			
			Pipe_SelectPipe(SIMAGE_EVENTS_PIPE);
			Pipe_SetInfiniteINRequests();
			Pipe_SetInterruptFreq(SImageEndpointIntFreq_EVENTS);

			puts_P(PSTR("Still Image Device Enumerated.\r\n"));
				
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Indicate device busy via the status LEDs */
			Bicolour_SetLeds(BICOLOUR_LED2_ORANGE);
			
			PIMA_Command = (PIMA_Container_t)
				{
					DataLength:    40,
					Type:          CType_CommandBlock,
					Code:          PIMA_GETDEVICEINFO,
					TransactionID: PIMA_TransactionID,
					Parameters:    {0, 0, 0, 0, 0}
				};
			
			Pipe_SelectPipe(SIMAGE_DATA_OUT_PIPE);
			Pipe_Unfreeze();

			uint8_t* CommandByte = (uint8_t*)&PIMA_Command;
			
			for (uint8_t Byte = 0; Byte < sizeof(PIMA_Container_t); Byte++)
			  Pipe_Write_Byte(*(CommandByte++));
			
			Pipe_FIFOCON_Clear();
			
			Pipe_Freeze();

			Pipe_SelectPipe(SIMAGE_DATA_IN_PIPE);
			Pipe_Unfreeze();
			
			bool InData = true;
			
			while (InData)
			{
				while (!(Pipe_ReadWriteAllowed()));
				
				Pipe_Ignore_DWord();
				
				if (Pipe_Read_Word_LE() == CType_ResponseBlock)
				  InData = false;
				  
				Pipe_FIFOCON_Clear();
			}

			while (!(Pipe_ReadWriteAllowed()));
			Pipe_FIFOCON_Clear();

			Pipe_Freeze();		

			/* Indicate device ready via the status LEDs */
			Bicolour_SetLeds(BICOLOUR_LED2_GREEN);
			
			break;
	}
}

uint8_t GetConfigDescriptorData(void)
{
	uint8_t* ConfigDescriptorData;
	uint16_t ConfigDescriptorSize;
	
	/* Get Configuration Descriptor size from the device */
	if (AVR_HOST_GetDeviceConfigDescriptor(&ConfigDescriptorSize, NULL) != HOST_SENDCONTROL_Sucessful)
	  return ControlError;
	
	/* Ensure that the Configuration Descriptor isn't too large */
	if (ConfigDescriptorSize > MAX_CONFIG_DESCRIPTOR_SIZE)
	  return DescriptorTooLarge;
	  
	/* Allocate enough memory for the entire config descriptor */
	ConfigDescriptorData = alloca(ConfigDescriptorSize);

	/* Retrieve the entire configuration descriptor into the allocated buffer */
	AVR_HOST_GetDeviceConfigDescriptor(&ConfigDescriptorSize, ConfigDescriptorData);
	
	/* Validate returned data - ensure first entry is a configuration header descriptor */
	if (DESCRIPTOR_TYPE(ConfigDescriptorData) != DTYPE_Configuration)
	  return ControlError;
	
	while (ConfigDescriptorSize)
	{
		/* Find next interface descriptor */
		while (ConfigDescriptorSize)
		{
			/* Get the next descriptor from the configuration descriptor data */
			AVR_HOST_GetNextDescriptor(&ConfigDescriptorSize, &ConfigDescriptorData);	  

			/* Check to see if the next descriptor is an interface descriptor, if so break out */
			if (DESCRIPTOR_TYPE(ConfigDescriptorData) == DTYPE_Interface)
			  break;
		}

		/* If reached end of configuration descriptor, error out */
		if (ConfigDescriptorSize == 0)
		  return InterfaceNotFound;	
		
		/* Check the descriptor's class/subclass/protocol, break out if class matches expected values */
		if ((DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).Class    == SIMAGE_CLASS)    &&
		    (DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).SubClass == SIMAGE_SUBCLASS) &&
		    (DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).Protocol == SIMAGE_PROTOCOL))
		{
			break;
		}
	}
	
	/* If reached end of configuration descriptor, error out */
	if (ConfigDescriptorSize == 0)
	  return InterfaceNotFound;
	
	/* Find the IN, OUT and EVENT endpoint descriptors after the still image interface descriptor */
	while (ConfigDescriptorSize)
	{
		/* Get the next descriptor from the configuration descriptor data */
		AVR_HOST_GetNextDescriptor(&ConfigDescriptorSize, &ConfigDescriptorData);	  		

		/* Check if current descritor is an endpoint descriptor */
		if (DESCRIPTOR_TYPE(ConfigDescriptorData) == DTYPE_Endpoint)
		{
			if (DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t).Attributes == EP_TYPE_BULK)
			{
				uint8_t  EPAddress = DESCRIPTOR_CAST(ConfigDescriptorData,
				                                     USB_Descriptor_Endpoint_t).EndpointAddress;
				uint16_t EPSize    = DESCRIPTOR_CAST(ConfigDescriptorData,
				                                     USB_Descriptor_Endpoint_t).EndpointSize;
			
				/* Set the appropriate endpoint data address based on the endpoint direction */
				if (EPAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
				{
					SImageEndpointNumber_IN  = EPAddress;
					SImageEndpointSize_IN    = EPSize;
				}
				else
				{
					SImageEndpointNumber_OUT = EPAddress;
					SImageEndpointSize_OUT   = EPSize;
				}
			}
			else if (DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t).Attributes == EP_TYPE_INTERRUPT)
			{
				uint8_t  EPAddress    = DESCRIPTOR_CAST(ConfigDescriptorData,
				                                        USB_Descriptor_Endpoint_t).EndpointAddress;
				uint16_t EPSize       = DESCRIPTOR_CAST(ConfigDescriptorData,
				                                        USB_Descriptor_Endpoint_t).EndpointSize;
				uint8_t EPPollingInt  = DESCRIPTOR_CAST(ConfigDescriptorData,
				                                        USB_Descriptor_Endpoint_t).PollingIntervalMS;
			
				/* If the endpoint is an IN type, store the endpoint data */
				if (EPAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
				{
					SImageEndpointNumber_EVENTS  = EPAddress;
					SImageEndpointSize_EVENTS    = EPSize;
					SImageEndpointIntFreq_EVENTS = EPPollingInt;
				}
			}
			
			/* If both data endpoints and the event endpoint found, exit the loop */
			if (SImageEndpointNumber_IN && SImageEndpointNumber_OUT && SImageEndpointNumber_EVENTS)
			  break;
		}
		/* If new interface descriptor found (indicating the end of the current interface), error out */
		else if (DESCRIPTOR_TYPE(ConfigDescriptorData) == DTYPE_Interface)
		  return NoEndpointFound;
	}	

	/* If reached end of configuration descriptor, error out */
	if (ConfigDescriptorSize == 0)
	  return NoEndpointFound;
	
	/* Valid data found, return success */
	return SuccessfulConfigRead;
}
