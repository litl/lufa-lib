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
	the standard mouse HID profile. It uses a HID parser for the HID
	reports, allowing for correct operation across all USB mice. This
	demo supports mice with a single HID report.
	
	Mouse movement and button presses are displayed on the board LEDs.

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

#include "MouseHostWithParser.h"

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
uint8_t          MouseDataEndpointNumber;
uint16_t         MouseDataEndpointSize;

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
				uint8_t LEDMask = LEDS_NO_LEDS;

				/* Create a pointer to walk through each of the HID report items */
				HID_ReportItem_t* ReportItem = NULL;

				/* Create buffer big enough for the report */
				uint8_t MouseReport[Pipe_BytesInPipe()];

				/* Load in the mouse report */
				Pipe_Read_Stream_LE(MouseReport, Pipe_BytesInPipe());
				
				/* Check each HID report item in turn, looking for mouse X/Y/button reports */
				for (uint8_t ReportNumber = 0; ReportNumber < HIDReportInfo.TotalReportItems; ReportNumber++)
				{
					/* Set temp report item pointer to the next report item */
					ReportItem = &HIDReportInfo.ReportItems[ReportNumber];

					if ((ReportItem->Attributes.Usage.Page       == USAGE_PAGE_BUTTON) &&
					    (ReportItem->ItemType                    == REPORT_ITEM_TYPE_In))
					{
						/* Get the mouse button value */
						GetReportItemInfo((void*)&MouseReport, ReportItem);
						
						/* If button is pressed, all LEDs are turned on */
						if (ReportItem->Value)
						  LEDMask = LEDS_ALL_LEDS;
					}
					else if ((ReportItem->Attributes.Usage.Page  == USAGE_PAGE_GENERIC_DCTRL) &&
							 (ReportItem->Attributes.Usage.Usage == USAGE_X)                  &&
					         (ReportItem->ItemType               == REPORT_ITEM_TYPE_In))
					{
						/* Get the mouse relative position value */
						GetReportItemInfo((void*)&MouseReport, ReportItem);
						
						/* Value is a signed 8-bit value, cast and set LED mask as appropriate */
						if ((int8_t)ReportItem->Value > 0)
						  LEDMask |= LEDS_LED1;
						else if ((int8_t)ReportItem->Value < 0)
						  LEDMask |= LEDS_LED2;
					}
					else if ((ReportItem->Attributes.Usage.Page  == USAGE_PAGE_GENERIC_DCTRL) &&
							 (ReportItem->Attributes.Usage.Usage == USAGE_Y)                  &&
					         (ReportItem->ItemType               == REPORT_ITEM_TYPE_In))
					{
						/* Get the mouse relative position value */
						GetReportItemInfo((void*)&MouseReport, ReportItem);
						
						/* Value is a signed 8-bit value, cast and set LED mask as appropriate */
						if ((int8_t)ReportItem->Value > 0)
						  LEDMask |= LEDS_LED3;
						else if ((int8_t)ReportItem->Value < 0)
						  LEDMask |= LEDS_LED4;
					}
				}
				
				/* Display the button information on the board LEDs */
				LEDs_SetAllLEDs(LEDMask);
					
				/* Clear the IN endpoint, ready for next data packet */
				Pipe_FIFOCON_Clear();
			}

			/* Freeze mouse data pipe */
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
	
	/* Get Configuration Descriptor size from the device */
	if (AVR_HOST_GetDeviceConfigDescriptor(&ConfigDescriptorSize, NULL) != HOST_SENDCONTROL_Successful)
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
		/* Get the next interface descriptor from the configuration descriptor */
		AVR_HOST_GetNextDescriptorOfType(&ConfigDescriptorSize, &ConfigDescriptorData, DTYPE_Interface);
	
		/* If reached end of configuration descriptor, error out */
		if (ConfigDescriptorSize == 0)
		  return NoHIDInterfaceFound;

		/* Check the HID descriptor class and protocol, break out if correct class/protocol interface found */
		if ((DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).Class == MOUSE_CLASS) &&
		    (DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_Interface_t).Protocol == MOUSE_PROTOCOL))
		{
			break;
		}
	}

	/* If reached end of configuration descriptor, error out */
	if (ConfigDescriptorSize == 0)
	  return NoHIDInterfaceFound;
	
	/* Get the next HID descriptor from the configuration descriptor data before the next interface descriptor*/
	AVR_HOST_GetNextDescriptorOfTypeBefore(&ConfigDescriptorSize, &ConfigDescriptorData, DTYPE_HID, DTYPE_Interface);
	
	/* If reached end of configuration descriptor, error out */
	if (ConfigDescriptorSize == 0)
	  return NoHIDDescriptorFound;

	/* Save the HID report length for later use */
	HIDReportSize = DESCRIPTOR_CAST(ConfigDescriptorData, USB_Descriptor_HID_t).HIDReportLength;		

	/* Find the next IN endpoint descriptor after the mouse interface descriptor */
	while (ConfigDescriptorSize)
	{
		/* Get the next endpoint descriptor from the configuration descriptor data before the next interface descriptor*/
		AVR_HOST_GetNextDescriptorOfTypeBefore(&ConfigDescriptorSize, &ConfigDescriptorData,
		                                       DTYPE_Endpoint, DTYPE_Interface);
	
		/* If reached end of configuration descriptor, error out */
		if (ConfigDescriptorSize == 0)
		  return NoEndpointFound;

		/* Break out of the loop and process the endpoint descriptor if it is of the IN type */
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
