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
	On connection to a USB mouse, the report items will be processed and
	printed as a formatted list through the USART before the mouse is
	fully enumerated.

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
			if ((ErrorCode = ProcessConfigurationDescriptor()) != SuccessfulConfigRead)
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
		
			puts_P(PSTR("Processing HID Report.\r\n"));

			/* LEDs one and two on to indicate busy processing */
			LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED2);

			/* Get and process the device's first HID report descriptor */
			if ((ErrorCode = GetHIDReportData()) != ParseSucessful)
			{
				puts_P(PSTR("Report Parse Error.\r\n"));
				printf_P(PSTR(" -- Error Code: %d\r\n"), ErrorCode);
			
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
					else if ((ReportItem->Attributes.Usage.Page   == USAGE_PAGE_GENERIC_DCTRL) &&
					         ((ReportItem->Attributes.Usage.Usage == USAGE_X)                  ||
					          (ReportItem->Attributes.Usage.Usage == USAGE_Y))                 &&
					         (ReportItem->ItemType                == REPORT_ITEM_TYPE_In))
					{
						/* Get the mouse relative position value */
						GetReportItemInfo((void*)&MouseReport, ReportItem);
						
						/* Value is a signed 8-bit value, cast as appropriate */
						int8_t DeltaMovement = (int8_t)ReportItem->Value;
						
						/* Determine if the report is for the X or Y delta movement */
						if (ReportItem->Attributes.Usage.Usage == USAGE_X)
						{
							/* Turn on the appropriate LED according to direction if the delta is non-zero */
							if (DeltaMovement)
							  LEDMask |= ((DeltaMovement > 0) ? LEDS_LED1 : LEDS_LED2);
						}
						else
						{
							/* Turn on the appropriate LED according to direction if the delta is non-zero */
							if (DeltaMovement)
							  LEDMask |= ((DeltaMovement > 0) ? LEDS_LED3 : LEDS_LED4);
						}
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
