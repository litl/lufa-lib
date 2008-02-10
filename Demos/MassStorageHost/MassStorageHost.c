/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Mass Storage host demonstration application. This gives a simple reference
	application for implementing a USB Mass Storage host, for USB storage devices
	using the standard Mass Storage USB profile.
	
	The first 512 bytes (boot sector) of an attached disk's memory will be dumped
	out of the serial port when it is attached to the AT90USB1287 AVR.
	
	Requires header files from the Mass Storage Device demonstation application.
*/

#include "MassStorageHost.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB MS Host App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_MassStore_Host   , TaskStatus: TASK_STOP },
};

/* Globals */
uint8_t  MassStoreEndpointNumber_IN;
uint8_t  MassStoreEndpointNumber_OUT;
uint16_t MassStoreEndpointSize_IN;
uint16_t MassStoreEndpointSize_OUT;
uint8_t  MassStore_NumberOfLUNs;

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

	/* Startup message */
	puts_P(PSTR(ESC_RESET ESC_BG_WHITE ESC_INVERSE_ON ESC_ERASE_DISPLAY
	       "MassStore Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_HOST, USB_OPT_REG_ENABLED);
	
	/* Scheduling routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR("Device Attached.\r\n"));
	Bicolour_SetLeds(BICOLOUR_NO_LEDS);
	
	/* Start USB management and Mass Storage tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
	Scheduler_SetTaskMode(USB_MassStore_Host, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	/* Stop USB management and Mass Storage tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_MassStore_Host, TASK_STOP);

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

TASK(USB_MassStore_Host)
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
				puts_P(PSTR("Control error."));

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

			/* Configure the data pipes */
			Pipe_ConfigurePipe(MASS_STORE_DATA_IN_PIPE, EP_TYPE_BULK, PIPE_TOKEN_IN,
			                   MassStoreEndpointNumber_IN, MassStoreEndpointSize_IN,
			                   PIPE_BANK_DOUBLE);

			Pipe_ConfigurePipe(MASS_STORE_DATA_OUT_PIPE, EP_TYPE_BULK, PIPE_TOKEN_OUT,
			                   MassStoreEndpointNumber_OUT, MassStoreEndpointSize_OUT,
			                   PIPE_BANK_DOUBLE);

			Pipe_SelectPipe(MASS_STORE_DATA_IN_PIPE);
			Pipe_SetInfiniteINRequests();

			puts_P(PSTR("Mass Storage Disk Enumerated.\r\n"));
				
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Indicate device busy via the status LEDs */
			Bicolour_SetLeds(BICOLOUR_LED2_ORANGE);
			
			/* Request to prepare the disk for use */
			USB_HostRequest = (USB_Host_Request_Header_t)
				{
					RequestType: (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
					RequestData: MASS_STORAGE_RESET,
					Value:       0,
					Index:       0,
					DataLength:  0,
				};

			/* Send the request, display error and wait for device detatch if request fails */
			if (USB_Host_SendControlRequest(NULL) != HOST_SENDCONTROL_Sucessful)
			{
				puts_P(PSTR("Control error."));

				/* Indicate error via status LEDs */
				Bicolour_SetLeds(BICOLOUR_LED1_RED);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}
			
			/* Request to retrieve the maximum LUN index from the device */
			USB_HostRequest = (USB_Host_Request_Header_t)
				{
					RequestType: (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
					RequestData: GET_MAX_LUN,
					Value:       0,
					Index:       0,
					DataLength:  1,
				};

			/* Send the request, display error and wait for device detatch if request fails */
			if (USB_Host_SendControlRequest(&MassStore_NumberOfLUNs) != HOST_SENDCONTROL_Sucessful)
			{
				puts_P(PSTR("Control error."));

				/* Indicate error via status LEDs */
				Bicolour_SetLeds(BICOLOUR_LED1_RED);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}
			
			/* Print number of LUNs detected in the attached device */
			printf_P(PSTR("Total LUNs: %d.\r\n"), (MassStore_NumberOfLUNs + 1));
			
			/* Get sense data from the device - it will not respond to requests until this is done */
			SCSI_Request_Sense_Response_t SenseData;
			if ((ErrorCode = MassStore_RequestSense(0, &SenseData)) != 0)
			{
				ShowDiskReadError(ErrorCode);
				break;
			}
			
			/* Set the prevent removal flag for the device, allowing it to be accessed */
			MassStore_PreventAllowMediumRemoval(0, true);
			
			puts_P(PSTR("Waiting until ready.\r\n"));
			
			/* Wait until disk ready */
			do
			{
				Serial_TxByte('.');
				MassStore_TestUnitReady(0);
			}
			while (SCSICommandStatus.Status != Command_Pass);
			
			puts_P(PSTR("\r\nRetrieving Capacity.\r\n"));

			/* Create new structure for the disk's capacity in blocks and block size */
			SCSI_Capacity_t DiskCapacity;

			/* Retrieve disk capacity */
			if ((ErrorCode = MassStore_ReadCapacity(0, &DiskCapacity)) != 0)
			{
				ShowDiskReadError(ErrorCode);
				break;
			}
			
			/* Display the disk capacity in blocks * block size bytes */
			printf_P(PSTR("Capacity: %u*%u bytes.\r\n"), DiskCapacity.Blocks, DiskCapacity.BlockSize);
			
			/* Create a new buffer capabable of holding a single block from the device */
			uint8_t BlockBuffer[DEVICE_BLOCK_SIZE];

			/* Read in the first 512 byte block from the device */
			if ((ErrorCode = MassStore_ReadDeviceBlock(0, 0, 1, BlockBuffer)) != 0)
			{
				ShowDiskReadError(ErrorCode);
				break;
			}
			
			puts_P(PSTR("Contents of first block:\r\n"));
			
			/* Print the block bytes out through the serial USART */
			for (uint16_t Byte = 0; Byte < DEVICE_BLOCK_SIZE; Byte++)
			  printf_P(PSTR("0x%.2X "), BlockBuffer[Byte]);
			
			/* Indicate device no longer busy */
			Bicolour_SetLeds(BICOLOUR_LED2_GREEN);			
			
			/* Wait until USB device disconnected */
			while (USB_IsConnected);
			
			break;
	}
}

void ShowDiskReadError(uint8_t ErrorCode)
{
	/* Display the error code */
	puts_P(PSTR(ESC_BG_RED "Command error.\r\n"));
	printf_P(PSTR("  -- Error Code: %d"), ErrorCode);

	/* Indicate device error via the status LEDs */
	Bicolour_SetLeds(BICOLOUR_LED1_RED);				

	/* Wait until USB device disconnected */
	while (USB_IsConnected);
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
		if ((DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).Class    == MASS_STORE_CLASS)    &&
		    (DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).SubClass == MASS_STORE_SUBCLASS) &&
		    (DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).Protocol == MASS_STORE_PROTOCOL))
		{
			break;
		}
	}
	
	/* If reached end of configuration descriptor, error out */
	if (ConfigDescriptorSize == 0)
	  return InterfaceNotFound;
	
	/* Find the IN and OUT endpoint descriptors after the mass storage interface descriptor */
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
					MassStoreEndpointNumber_IN  = EPAddress;
					MassStoreEndpointSize_IN    = EPSize;
				}
				else
				{
					MassStoreEndpointNumber_OUT = EPAddress;
					MassStoreEndpointSize_OUT   = EPSize;
				}

				/* If both data endpoints found, exit the loop */
				if (MassStoreEndpointNumber_IN && MassStoreEndpointNumber_OUT)
				  break;
			}
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
