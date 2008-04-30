/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Keyboard host demonstration application. This gives a simple reference
	application for implementing a USB Keyboard host, for USB keyboards using
	the standard Keyboard HID profile. It uses a HID parser for the HID reports,
	allowing for correct operation across all USB keyboards. This demo supports
	keyboards with a single HID report.
	
	Pressed alpha-numeric, enter or space key is transmitted through the serial
	USART at serial settings 9600, 8, N, 1. On connection to a USB keyboard, the
	report items will be processed and printed as a formatted list through the
	USART before the keyboard is fully enumerated.

	Currently only single interface keyboards are supported.
*/

/*
	USB Mode:           Host
	USB Class:          Human Interface Device (HID)
	USB Subclass:       Keyboard
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables 
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/

#include "KeyboardHostWithParser.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB KBD Host App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Keyboard_Host    , TaskStatus: TASK_STOP },
};

/* Globals */
uint8_t          KeyboardDataEndpointNumber;
uint16_t         KeyboardDataEndpointSize;

uint16_t         HIDReportSize;
HID_ReportInfo_t HIDReportInfo;

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
	       "Keyboard Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR("Device Attached.\r\n"));
	LEDs_SetAllLEDs(LEDS_NO_LEDS);

	/* Start keyboard and USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
	Scheduler_SetTaskMode(USB_Keyboard_Host, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	/* Stop keyboard and USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_Keyboard_Host, TASK_STOP);

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
			
			/* Configure the keyboard data pipe */
			Pipe_ConfigurePipe(KEYBOARD_DATAPIPE, EP_TYPE_INTERRUPT, PIPE_TOKEN_IN,
			                   KeyboardDataEndpointNumber, KeyboardDataEndpointSize, PIPE_BANK_SINGLE);

			Pipe_SetInfiniteINRequests();
		
			puts_P(PSTR("Processing HID Report.\r\n"));

			/* LEDs one and two on to indicate busy processing */
			LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED2);

			/* Get and process the device's first HID report descriptor */
			if ((ErrorCode = GetHIDReportData()) != ParseSucessful)
			{
				puts_P(PSTR("Report Parse Error.\r\n"));
			
				/* Indicate error via status LEDs */
				LEDs_SetAllLEDs(LEDS_LED1);
				
				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;			
			}

			puts_P(PSTR("Dumping HID Report Items.\r\n"));
			
			/* LEDs one, two and four on to indicate busy dumping descriptor data */
			LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED2 | LEDS_LED4);

			/* Loop through each of the loaded HID report items in the processed item structure */
			for (uint8_t ItemIndex = 0; ItemIndex < HIDReportInfo.TotalReportItems; ItemIndex++)
			{
				/* Create pointer to the current report info structure */
				HID_ReportItem_t* RItem = &HIDReportInfo.ReportItems[ItemIndex];
				
				/* Print out each report item's details */
				printf_P(PSTR("  Item %d:\r\n"
				              "    Type:       %s\r\n"
				              "    Flags:      %d\r\n"
				              "    BitOffset:  %d\r\n"
							  "    BitSize:    %d\r\n"
							  "    Coll Ptr:   %d\r\n"
							  "    Coll Usage: %d\r\n"
							  "    Coll Prnt:  %d\r\n"
							  "    Usage Page: %d\r\n"
							  "    Usage:      %d\r\n"
							  "    Usage Min:  %d\r\n"
							  "    Usage Max:  %d\r\n"
							  "    Unit Type:  %d\r\n"
							  "    Unit Exp:   %d\r\n"
							  "    Log Min:    %d\r\n"
							  "    Log Max:    %d\r\n"
							  "    Phy Min:    %d\r\n"
							  "    Phy Max:    %d\r\n"), ItemIndex,
				                                         ((RItem->ItemType == REPORT_ITEM_TYPE_In) ? "IN" : "OUT"),
							                             RItem->ItemFlags,
				                                         RItem->BitOffset,
				                                         RItem->Attributes.BitSize,
														 RItem->CollectionPath,
														 RItem->CollectionPath->Usage,
														 RItem->CollectionPath->Parent,
				                                         RItem->Attributes.Usage.Page,
				                                         RItem->Attributes.Usage.Usage,
				                                         RItem->Attributes.Usage.Minimum,
				                                         RItem->Attributes.Usage.Maximum,
				                                         RItem->Attributes.Unit.Type,
				                                         RItem->Attributes.Unit.Exponent,
				                                         RItem->Attributes.Logical.Minimum,
				                                         RItem->Attributes.Logical.Maximum,
				                                         RItem->Attributes.Physical.Minimum,
				                                         RItem->Attributes.Physical.Maximum);

				/* Toggle status LED to indicate busy */
				if (LEDs_GetLEDs() & LEDS_LED4)
				  LEDs_TurnOffLEDs(LEDS_LED4);
				else
				  LEDs_TurnOnLEDs(LEDS_LED4);
			}
			
			/* All LEDs off - ready to indicate keypresses */
			LEDs_SetAllLEDs(LEDS_NO_LEDS);

			puts_P(PSTR("Keyboard Enumerated.\r\n"));

			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Select and unfreeze keyboard data pipe */
			Pipe_SelectPipe(KEYBOARD_DATAPIPE);	
			Pipe_Unfreeze();

			/* Check if data has been recieved from the attached keyboard */
			if (Pipe_ReadWriteAllowed())
			{
				/* Create a pointer to walk through each of the HID report items */
				HID_ReportItem_t* ReportItem = NULL;

				/* Create buffer big enough for the report */
				uint8_t KeyboardReport[Pipe_BytesInPipe()];

				/* Load in the keyboard report */
				Pipe_Read_Stream_LE(KeyboardReport, Pipe_BytesInPipe());
				
				/* Clear the IN endpoint, ready for next data packet */
				Pipe_FIFOCON_Clear();

				/* Check each HID report item in turn, looking for keyboard scan code reports */
				for (uint8_t ReportNumber = 0; ReportNumber < HIDReportInfo.TotalReportItems; ReportNumber++)
				{
					/* Set temp report item pointer to the next report item */
					ReportItem = &HIDReportInfo.ReportItems[ReportNumber];

					/* Check if the current report item is a keyboard scancode */
					if ((ReportItem->Attributes.Usage.Page      == USAGEPAGE_KEYBOARD) &&
					    (ReportItem->Attributes.BitSize         == 8)                  &&
					    (ReportItem->Attributes.Logical.Maximum > 1)                   &&
					    (ReportItem->ItemType                   == REPORT_ITEM_TYPE_In))
					{
						/* Retrieve the keyboard scancode from the report data retrieved from the device */
						GetReportItemInfo((void*)&KeyboardReport, ReportItem);
						
						/* If scancode is non-zero, a key is being pressed */
						if (ReportItem->Value)
						{
							/* Toggle status LED to indicate keypress */
							if (LEDs_GetLEDs() & LEDS_LED2)
							  LEDs_TurnOffLEDs(LEDS_LED2);
							else
							  LEDs_TurnOnLEDs(LEDS_LED2);
						}
						
						/* Once a scancode is found, stop scanning through the report items */
						break;
					}
				}
			}

			/* Freeze keyboard data pipe */
			Pipe_Freeze();
			break;
	}
}

uint8_t GetHIDReportData(void)
{
	/* Create a buffer big enough to hold the entire returned HID report */
	uint8_t HIDReportData[HIDReportSize];
	
	USB_HostRequest = (USB_Host_Request_Header_t)
		{
			RequestType: (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE),
			RequestData: REQ_GetDescriptor,
			Value:       (DTYPE_Report << 8),
			Index:       0,
			DataLength:  HIDReportSize,
		};

	/* Send control request to retrieve the HID report from the attached device */
	if (USB_Host_SendControlRequest(HIDReportData) != HOST_SENDCONTROL_Successful)
	  return ParseControlError;

	/* Send the HID report to the parser for processing */
	if (ProcessHIDReport((void*)&HIDReportData, HIDReportSize, &HIDReportInfo) != HID_PARSE_Sucessful)
	  return ParseError;
	
	return ParseSucessful;
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
	
	/* Get the keyboard interface from the configuration descriptor */
	if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData, NextKeyboardInterface)))
	{
		/* Descriptor not found, error out */
		return NoHIDInterfaceFound;
	}
	
	/* Get the keyboard interface's HID descriptor */
	if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData, NextHID)))
	{
		/* Descriptor not found, error out */
		return NoHIDDescriptorFound;
	}

	/* Save the HID report size for later use */
	HIDReportSize = DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_HID_t).HIDReportLength;

	/* Get the keyboard interface's data endpoint descriptor */
	if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
	                                                NextInterfaceKeyboardDataEndpoint)))
	{
		/* Descriptor not found, error out */
		return NoEndpointFound;
	}
	
	/* Retrieve the endpoint address from the endpoint descriptor */
	KeyboardDataEndpointNumber = DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t).EndpointAddress;
	KeyboardDataEndpointSize   = DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t).EndpointSize;
			
	/* Valid data found, return success */
	return SuccessfulConfigRead;
}

DESCRIPTOR_COMPARATOR(NextKeyboardInterface)
{
	/* Descriptor Search Comparitor Function - find next keyboard class interface descriptor */

	if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Interface)
	{
		/* Check the HID descriptor class and protocol, break out if correct class/protocol interface found */
		if ((DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).Class    == KEYBOARD_CLASS) &&
		    (DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Interface_t).Protocol == KEYBOARD_PROTOCOL))
		{
			return Descriptor_Search_Found;
		}
	}
	
	return Descriptor_Search_NotFound;
}

DESCRIPTOR_COMPARATOR(NextInterfaceKeyboardDataEndpoint)
{
	/* Descriptor Search Comparitor Function - find next interface endpoint descriptor before next interface descriptor */

	if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Endpoint)
	{
		if (DESCRIPTOR_CAST(CurrentDescriptor, USB_Descriptor_Endpoint_t).EndpointAddress & ENDPOINT_DESCRIPTOR_DIR_IN)
		  return Descriptor_Search_Found;
	}
	else if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_Interface)
	{
		return Descriptor_Search_Fail;
	}

	return Descriptor_Search_NotFound;
}

DESCRIPTOR_COMPARATOR(NextHID)
{
	/* Descriptor Search Comparitor Function - find next HID descriptor */

	if (DESCRIPTOR_TYPE(CurrentDescriptor) == DTYPE_HID)
	  return Descriptor_Search_Found;
	else
	  return Descriptor_Search_NotFound;	  
}
