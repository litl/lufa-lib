/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	CDC host demonstration application. This gives a simple reference application
	for implementing a USB CDC host, for CDC devices using the standard ACM profile.
	
	This demo prints out received CDC data through the serial port.
	
	Not that this demo is only compatible with devices which report the correct CDC
	and ACM class, subclass and protocol values. Most USB-Serial cables have vendor
	specific features, thus use vendor-specfic class/subclass/protocol codes to force
	the user to use specialized drivers. This demo is not compaible with such devices.
*/

/*
	USB Mode:           Host
	USB Class:          Communications Device Class (CDC)
	USB Subclass:       Abstract Control Model (ACM)
	Relevant Standards: USBIF CDC Class Standard
	Usable Speeds:      Full Speed Mode
*/

#include "CDCHost.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB CDC Host App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_CDC_Host         , TaskStatus: TASK_STOP },
};

/* Globals */
uint8_t  CDCDataEndpointInNumber;
uint16_t CDCDataEndpointInSize;
uint8_t  CDCDataEndpointOutNumber;
uint16_t CDCDataEndpointOutSize;
uint8_t  CDCDataEndpointNotificationNumber;
uint16_t CDCDataEndpointNotificationSize;

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
	       "CDC Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR("Device Attached.\r\n"));
	LEDs_SetAllLEDs(LEDS_NO_LEDS);

	/* Start keyboard and USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
	Scheduler_SetTaskMode(USB_CDC_Host, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	/* Stop keyboard and USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_CDC_Host, TASK_STOP);

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

TASK(USB_CDC_Host)
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
			if ((ErrorCode = USB_Host_SendControlRequest(NULL)) != HOST_SENDCONTROL_Successful)
			{
				puts_P(PSTR("Control Error (Set Configuration).\r\n"));
				printf_P(PSTR(" -- Error Code: %d\r\n"), ErrorCode);

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

			/* Configure the data pipes */
			Pipe_ConfigurePipe(CDC_DATAPIPE_IN, EP_TYPE_BULK, PIPE_TOKEN_IN,
			                   CDCDataEndpointInNumber, CDCDataEndpointInSize, PIPE_BANK_SINGLE);

			Pipe_ConfigurePipe(CDC_DATAPIPE_OUT, EP_TYPE_BULK, PIPE_TOKEN_OUT,
			                   CDCDataEndpointOutNumber, CDCDataEndpointOutSize, PIPE_BANK_SINGLE);
							   
			Pipe_ConfigurePipe(CDC_NOTIFICATIONPIPE, EP_TYPE_INTERRUPT, PIPE_TOKEN_IN,
			                   CDCDataEndpointNotificationNumber, CDCDataEndpointNotificationSize, PIPE_BANK_SINGLE);							   

			Pipe_SelectPipe(CDC_DATAPIPE_IN);
			Pipe_SetInfiniteINRequests();
			Pipe_Unfreeze();
		
			Pipe_SelectPipe(CDC_NOTIFICATIONPIPE);
			Pipe_SetInfiniteINRequests();

			puts_P(PSTR("CDC Device Enumerated.\r\n"));
				
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Select and the data IN pipe */
			Pipe_SelectPipe(CDC_DATAPIPE_IN);

			/* Check if data is in the pipe */
			if (Pipe_ReadWriteAllowed())
			{
				/* Get the length of the pipe data, and create a new buffer to hold it */
				uint16_t BufferLength = Pipe_BytesInPipe();
				uint8_t Buffer[BufferLength];
				
				/* Read in the pipe data to the tempoary buffer */
				Pipe_Read_Stream_LE(Buffer, BufferLength);
				
				/* Clear the pipe after it is read, ready for the next packet */
				Pipe_FIFOCON_Clear();
				
				/* Print out the buffer contents to the USART */
				for (uint16_t BufferByte = 0; BufferByte < BufferLength; BufferByte++)
				  printf_P(PSTR("%c"), Buffer[BufferByte]);
			}

			/* Select and unfreeze the notification pipe */
			Pipe_SelectPipe(CDC_NOTIFICATIONPIPE);
			Pipe_Unfreeze();
			
			/* Check if data is in the pipe */
			if (Pipe_ReadWriteAllowed())
			{
				/* Discard the event notification */
				Pipe_FIFOCON_Clear();
			}
			
			/* Freeze notification IN pipe after use */
			Pipe_Freeze();
						
			break;
	}
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
	
	/* Get the CDC interface from the configuration descriptor */
	if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData, NextCDCInterface)))
	{
		/* Descriptor not found, error out */
		return NoCDCInterfaceFound;
	}

	/* Get the IN and OUT data endpoints for the CDC interface */
	while (!(CDCDataEndpointInNumber && CDCDataEndpointOutNumber && CDCDataEndpointNotificationNumber))
	{
		/* Fetch the next bulk endpoint from the current mass storage interface */
		if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
		                                                NextInterfaceCDCDataEndpoint)))
		{
			/* Descriptor not found, error out */
			return NoEndpointFound;
		}
		
		uint8_t  EPAddress = DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t).EndpointAddress;
		uint16_t EPSize    = DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t).EndpointSize;

		/* Check if the found endpoint is a interrupt or bulk type descriptor */
		if (DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t).Attributes == EP_TYPE_INTERRUPT)
		{
			/* If the endpoint is a IN type interrupt endpoint, set appropriate globals */
			if (EPAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
			{
				CDCDataEndpointNotificationNumber = EPAddress;
				CDCDataEndpointNotificationSize   = EPSize;
			}
		}
		else
		{
			/* Check if the endpoint is a bulk IN or bulk OUT endpoint, set appropriate globals */
			if (EPAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
			{
				CDCDataEndpointInNumber  = EPAddress;
				CDCDataEndpointInSize    = EPSize;
			}
			else
			{
				CDCDataEndpointOutNumber = EPAddress;
				CDCDataEndpointOutSize   = EPSize;
			}
		}
	}

	/* Valid data found, return success */
	return SuccessfulConfigRead;
}

DESCRIPTOR_COMPARATOR(NextCDCInterface)
{
	/* Descriptor Search Comparitor Function - find next keyboard class interface descriptor */

	if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Interface)
	{
		/* Check the HID descriptor class and protocol, break out if correct class/protocol interface found */
		if ((DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).Class    == CDC_CLASS)    &&
		    (DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).SubClass == CDC_SUBCLASS) &&
		    (DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).Protocol == CDC_PROTOCOL))
		{
			return Descriptor_Search_Found;
		}
	}
	
	return Descriptor_Search_NotFound;
}

DESCRIPTOR_COMPARATOR(NextInterfaceCDCDataEndpoint)
{
	/* Descriptor Search Comparitor Function - find next interface bulk or interrupt endpoint descriptor before
	                                           next interface descriptor */

	if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Endpoint)
	{
		if ((DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Endpoint_t).Attributes == EP_TYPE_BULK) ||
		    (DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Endpoint_t).Attributes == EP_TYPE_INTERRUPT))
		{
			return Descriptor_Search_Found;
		}
	}
	else if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Interface)
	{
		return Descriptor_Search_Fail;
	}

	return Descriptor_Search_NotFound;
}
