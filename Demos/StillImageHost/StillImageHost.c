/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/*
	Still Image host demonstration application. This gives a simple reference
	application for implementing a Still Image host, for USB devices such as
	digital cameras.
	
	This demo will enumerate an attached USB Still Image device, print out its
	information structure, open a session with the device and finally close the
	session.
*/

/*
	USB Mode:           Host
	USB Class:          Still Image Device
	USB Subclass:       N/A
	Relevant Standards: USBIF Still Image Class Specification
	                    PIMA 15740 Specification
	Usable Speeds:      Full Speed Mode
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
	
	/* Start USB management task to enumerate the device */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	/* Stop USB management and Still Image tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_SImage_Host, TASK_STOP);

	puts_P(PSTR("\r\nDevice Unattached.\r\n"));
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_DeviceEnumerationComplete)
{
	/* Once device is fully enumerated, start the Still Image Host task */
	Scheduler_SetTaskMode(USB_SImage_Host, TASK_RUN);
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

	switch (USB_HostState)
	{
		case HOST_STATE_Addressed:
			/* Standard request to set the device configuration to configuration 1 */
			USB_HostRequest = (USB_Host_Request_Header_t)
				{
					bmRequestType: (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE),
					bRequest:      REQ_SetConfiguration,
					wValue:        1,
					wIndex:        0,
					wLength:       0,
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

			puts_P(PSTR("Still Image Device Enumerated.\r\n"));
				
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Indicate device busy via the status LEDs */
			LEDs_SetAllLEDs(LEDS_LED3 | LEDS_LED4);

			puts_P(PSTR("Retrieving Device Info...\r\n"));
			
			PIMA_SendBlock = (PIMA_Container_t)
				{
					DataLength:    PIMA_COMMAND_SIZE(0),
					Type:          CType_CommandBlock,
					Code:          PIMA_OPERATION_GETDEVICEINFO,
					TransactionID: 0x00000000,
					Params:        {},
				};
			
			/* Send the GETDEVICEINFO block */
			SImage_SendBlockHeader();
			
			/* Recieve the response data block */
			if ((ErrorCode = SImage_RecieveBlockHeader()) != NoError)
			{
				ShowCommandError(ErrorCode, false);
				break;
			}
			
			/* Calculate the size of the returned device info data structure */
			uint16_t DeviceInfoSize = (PIMA_RecievedBlock.DataLength - PIMA_COMMAND_SIZE(0));
			
			/* Create a buffer large enough to hold the entire device info */
			uint8_t DeviceInfo[DeviceInfoSize];

			/* Read in the data block data (containing device info) */
			SImage_ReadData(DeviceInfo, DeviceInfoSize);
			
			/* Once all the data has been read, the pipe must be cleared before the response can be sent */
			Pipe_ClearCurrentBank();
			
			/* Create a pointer for walking through the info dataset */
			uint8_t* DeviceInfoPos = DeviceInfo;
			
			/* Skip over the data before the unicode device information strings */
			DeviceInfoPos += 8;                                      // Skip to VendorExtensionDesc String
			DeviceInfoPos += ((*DeviceInfoPos << 1) + 1);            // Skip over VendorExtensionDesc String
			DeviceInfoPos += 2;                                      // Skip over FunctionalMode
			DeviceInfoPos += (4 + (*(uint32_t*)DeviceInfoPos << 1)); // Skip over OperationCode Array
			DeviceInfoPos += (4 + (*(uint32_t*)DeviceInfoPos << 1)); // Skip over EventCode Array
			DeviceInfoPos += (4 + (*(uint32_t*)DeviceInfoPos << 1)); // Skip over DevicePropCode Array
			DeviceInfoPos += (4 + (*(uint32_t*)DeviceInfoPos << 1)); // Skip over ObjectFormatCode Array
			DeviceInfoPos += (4 + (*(uint32_t*)DeviceInfoPos << 1)); // Skip over ObjectFormatCode Array
			
			/* Extract and convert the Manufacturer Unicode string to ASCII and print it through the USART */
			char Manufacturer[*DeviceInfoPos];
			UnicodeToASCII(DeviceInfoPos, Manufacturer);
			printf_P(PSTR("   Manufacturer: %s\r\n"), Manufacturer);

			DeviceInfoPos += ((*DeviceInfoPos << 1) + 1);            // Skip over Manufacturer String

			/* Extract and convert the Model Unicode string to ASCII and print it through the USART */
			char Model[*DeviceInfoPos];
			UnicodeToASCII(DeviceInfoPos, Model);
			printf_P(PSTR("   Model: %s\r\n"), Model);

			DeviceInfoPos += ((*DeviceInfoPos << 1) + 1);            // Skip over Model String

			/* Extract and convert the Device Version Unicode string to ASCII and print it through the USART */
			char DeviceVersion[*DeviceInfoPos];
			UnicodeToASCII(DeviceInfoPos, DeviceVersion);
			printf_P(PSTR("   Device Version: %s\r\n"), DeviceVersion);

			/* Recieve the final response block from the device */
			if ((ErrorCode = SImage_RecieveBlockHeader()) != NoError)
			{
				ShowCommandError(ErrorCode, false);
				break;
			}
			
			/* Verify that the command completed successfully */
			if ((PIMA_RecievedBlock.Type != CType_ResponseBlock) || (PIMA_RecievedBlock.Code != PIMA_RESPONSE_OK))
			{
				ShowCommandError(PIMA_RecievedBlock.Code, true);
				break;
			}
			
			puts_P(PSTR("Opening Session...\r\n"));
			
			PIMA_SendBlock = (PIMA_Container_t)
				{
					DataLength:    PIMA_COMMAND_SIZE(1),
					Type:          CType_CommandBlock,
					Code:          PIMA_OPERATION_OPENSESSION,
					TransactionID: 0x00000000,
					Params:        {0x00000001},
				};
			
			/* Send the OPENSESSION block, open a session with an ID of 0x0001 */
			SImage_SendBlockHeader();
			
			/* Recieve the response block from the device */
			if ((ErrorCode = SImage_RecieveBlockHeader()) != NoError)
			{
				ShowCommandError(ErrorCode, false);
				break;
			}
			
			/* Verify that the command completed successfully */
			if ((PIMA_RecievedBlock.Type != CType_ResponseBlock) || (PIMA_RecievedBlock.Code != PIMA_RESPONSE_OK))
			{
				ShowCommandError(PIMA_RecievedBlock.Code, true);
				break;
			}

			puts_P(PSTR("Closing Session...\r\n"));

			PIMA_SendBlock = (PIMA_Container_t)
				{
					DataLength:    PIMA_COMMAND_SIZE(1),
					Type:          CType_CommandBlock,
					Code:          PIMA_OPERATION_CLOSESESSION,
					TransactionID: 0x00000001,
					Params:        {0x00000001},
				};
			
			/* Send the CLOSESESSION block, close the session with an ID of 0x0001 */
			SImage_SendBlockHeader();
			
			/* Recieve the response block from the device */
			if ((ErrorCode = SImage_RecieveBlockHeader()) != NoError)
			{
				ShowCommandError(ErrorCode, false);
				break;
			}

			/* Verify that the command completed successfully */
			if ((PIMA_RecievedBlock.Type != CType_ResponseBlock) || (PIMA_RecievedBlock.Code != PIMA_RESPONSE_OK))
			{
				ShowCommandError(PIMA_RecievedBlock.Code, true);
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

void UnicodeToASCII(uint8_t* UnicodeString, char* Buffer)
{
	/* Get the number of characters in the string, skip to the start of the string data */
	uint8_t CharactersRemaining = *(UnicodeString++);
	
	/* Loop through the entire unicode string */
	while (CharactersRemaining--)
	{
		/* Load in the next unicode character (only the lower byte, only Unicode coded ASCII supported) */
		*(Buffer++) = *UnicodeString;
		
		/* Jump to the next unicode character */
		UnicodeString += 2;
	}
	
	/* Null terminate the string */
	*Buffer = 0;
}

void ShowCommandError(uint8_t ErrorCode, bool ResponseCodeError)
{
	char* FailureType = ((ResponseCodeError) ? PSTR("Response Code != OK") : PSTR("Transaction Fail"));

	printf_P(PSTR(ESC_BG_RED "Command Error (%S).\r\n"), FailureType);
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);
			
	/* Indicate error via status LEDs */
	LEDs_SetAllLEDs(LEDS_LED1);
				
	/* Wait until USB device disconnected */
	while (USB_IsConnected);
}
