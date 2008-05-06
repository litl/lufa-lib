/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Keyboard host demonstration application, using pipe interrupts. This gives
	a simple reference application for implementing a USB Keyboard host utilizing
	the MyUSB pipe interrupt system, for USB keyboards using the standard Keyboard
	HID profile.
	
	Pressed alpha-numeric, enter or space key is transmitted through the serial
	USART at serial settings 9600, 8, N, 1.

	This uses a naive method where the returned report structure is assumed.
	A better implementation uses the HID report parser for correct report data
	processing across all compatable keyboards, as shown in the
	KeyboardHostWithParser demo application.

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

#include "KeyboardHostViaInt.h"

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
uint8_t  KeyboardDataEndpointNumber;
uint16_t KeyboardDataEndpointSize;
uint8_t  KeyboardDataEndpointPollMS;

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

			/* Configure the keyboard data pipe */
			Pipe_ConfigurePipe(KEYBOARD_DATAPIPE, EP_TYPE_INTERRUPT, PIPE_TOKEN_IN,
			                   KeyboardDataEndpointNumber, KeyboardDataEndpointSize, PIPE_BANK_SINGLE);

			Pipe_SetInfiniteINRequests();

			/* Unfreeze the pipe and set the pipe interrupt frequency */
			Pipe_SetInterruptFreq(KeyboardDataEndpointPollMS);
			Pipe_Unfreeze();

			/* Enable the pipe IN interrupt for the data pipe */
			USB_INT_Enable(PIPE_INT_IN);
			
			puts_P(PSTR("Keyboard Enumerated.\r\n"));

			USB_HostState = HOST_STATE_Ready;
			break;
	}
}

ISR(ENDPOINT_PIPE_vect)
{
	USB_KeyboardReport_Data_t KeyboardReport;
	char                      PressedKey = 0;

	/* Check to see if the keyboard data pipe has caused the interrupt */
	if (Pipe_HasPipeInterrupted(KEYBOARD_DATAPIPE))
	{
		/* Clear the pipe interrupt, and select the keyboard pipe */
		Pipe_ClearPipeInterrupt(KEYBOARD_DATAPIPE);
		Pipe_SelectPipe(KEYBOARD_DATAPIPE);	

		/* Check to see if the pipe IN interrupt has fired */
		if (USB_INT_HasOccurred(PIPE_INT_IN) && USB_INT_IsEnabled(PIPE_INT_IN))
		{
			/* Clear interrupt flag */
			USB_INT_Clear(PIPE_INT_IN);		

			/* Read in keyboard report data */
			KeyboardReport.Modifier = Pipe_Read_Byte();
			Pipe_Ignore_Byte();
			KeyboardReport.KeyCode  = Pipe_Read_Byte();
						
			LEDs_ChangeLEDs(LEDS_LED1, (KeyboardReport.Modifier) ? LEDS_LED1 : 0);
						
			/* Check if a key has been pressed */
			if (KeyboardReport.KeyCode)
			{
				/* Toggle status LED to indicate keypress */
				if (LEDs_GetLEDs() & LEDS_LED4)
				  LEDs_TurnOffLEDs(LEDS_LED4);
				else
				  LEDs_TurnOnLEDs(LEDS_LED4);

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
			Pipe_FIFOCON_Clear();
		}
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
	
	/* Get the keyboard interface from the configuration descriptor */
	if ((ErrorCode = USB_Host_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData, NextKeyboardInterface)))
	{
		/* Descriptor not found, error out */
		return NoHIDInterfaceFound;
	}

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
