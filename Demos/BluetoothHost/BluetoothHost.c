/*
             LUFA Library
     Copyright (C) Dean Camera, 2009.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2009  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
	Bluetooth Dongle host demo application.
	
	** NOT CURRENTLY FUNCTIONAL - DO NOT USE **
*/

/*
	USB Mode:           Host
	USB Class:          Bluetooth USB Transport Class
	USB Subclass:       Bluetooth HCI USB Transport
	Relevant Standards: Bluetooth 2.0 Standard
	                    Bluetooth 2.0 HCI USB Transport Layer Addendum
						Bluetooth Assigned Numbers (Baseband)
	Usable Speeds:      Full Speed Mode
*/

#include "BluetoothHost.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,    "LUFA BT Host App");
BUTTLOADTAG(BuildTime,   __TIME__);
BUTTLOADTAG(BuildDate,   __DATE__);
BUTTLOADTAG(LUFAVersion, "LUFA V" LUFA_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          ,   TaskStatus: TASK_STOP },
	{ Task: USB_Bluetooth_Host   ,   TaskStatus: TASK_STOP },
	{ Task: Bluetooth_ProcessData,   TaskStatus: TASK_STOP },
	{ Task: Bluetooth_ProcessEvents, TaskStatus: TASK_STOP },
};

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	SerialStream_Init(9600, false);
	LEDs_Init();
	
	/* Indicate USB not ready */
	UpdateStatus(Status_USBNotReady);

	/* Startup message */
	puts_P(PSTR(ESC_RESET ESC_BG_WHITE ESC_INVERSE_ON ESC_ERASE_DISPLAY
	       "Bluetooth Host Demo running.\r\n" ESC_INVERSE_OFF));
		   
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();
	
	/* Scheduling routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_DeviceAttached)
{
	puts_P(PSTR("Device Attached.\r\n"));
	UpdateStatus(Status_USBEnumerating);
	
	/* Start USB management task to enumerate the device */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);
}

EVENT_HANDLER(USB_DeviceUnattached)
{
	/* Stop USB management and Bluetooth tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(USB_Bluetooth_Host, TASK_STOP);
	Scheduler_SetTaskMode(Bluetooth_ProcessData, TASK_STOP);
	Scheduler_SetTaskMode(Bluetooth_ProcessEvents, TASK_STOP);

	puts_P(PSTR("\r\nDevice Unattached.\r\n"));
	UpdateStatus(Status_USBNotReady);
}

EVENT_HANDLER(USB_DeviceEnumerationComplete)
{
	/* Once device is fully enumerated, start the Bluetooth Host task */
	Scheduler_SetTaskMode(USB_Bluetooth_Host, TASK_RUN);

	UpdateStatus(Status_USBReady);
}

EVENT_HANDLER(USB_HostError)
{
	USB_ShutDown();

	puts_P(PSTR(ESC_BG_RED "Host Mode Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);

	UpdateStatus(Status_HardwareError);
	for(;;);
}

EVENT_HANDLER(USB_DeviceEnumerationFailed)
{
	puts_P(PSTR(ESC_BG_RED "Dev Enum Error\r\n"));
	printf_P(PSTR(" -- Error Code %d\r\n"), ErrorCode);
	printf_P(PSTR(" -- In State %d\r\n"), USB_HostState);
	
	UpdateStatus(Status_EnumerationError);
}

TASK(USB_Bluetooth_Host)
{
	uint8_t ErrorCode;

	switch (USB_HostState)
	{
		case HOST_STATE_Addressed:
			puts_P(PSTR("Getting Device Data.\r\n"));
		
			/* Get and process the configuration descriptor data */
			if ((ErrorCode = ProcessDeviceDescriptor()) != SuccessfulDeviceRead)
			{
				if (ErrorCode == ControlErrorDuringDeviceRead)
				  puts_P(PSTR("Control Error (Get Device).\r\n"));
				else
				  puts_P(PSTR("Invalid Device.\r\n"));

				printf_P(PSTR(" -- Error Code: %d\r\n"), ErrorCode);
				
				/* Indicate error via status LEDs */
				LEDs_SetAllLEDs(LEDS_LED1);

				/* Wait until USB device disconnected */
				while (USB_IsConnected);
				break;
			}

			puts_P(PSTR("Bluetooth Dongle Detected.\r\n"));

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
				if (ErrorCode == ControlErrorDuringConfigRead)
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

			puts_P(PSTR("Bluetooth Dongle Enumerated.\r\n"));
			
			USB_HostState = HOST_STATE_Ready;
			break;
		case HOST_STATE_Ready:
			/* Indicate device busy via the status LEDs */
			LEDs_SetAllLEDs(LEDS_LED3 | LEDS_LED4);

			static bool HasReset = false;
			
			if (HasReset)
			  return;
			else
			  HasReset = true;

			Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
			Pipe_Unfreeze();

			Bluetooth_Command_ResetBaseBand();
			Bluetooth_Command_WriteClassOfDevice(DEVICE_CLASS_MAJOR_COMPUTER | DEVICE_CLASS_MINOR_COMPUTER_PALM);

			char LocalName[] = "LUFA Bluetooth Demo";
			Bluetooth_Command_SetLocalName(LocalName);
			printf_P(PSTR("Device Name: %s\r\n"), LocalName);

			puts_P(PSTR("Allowing remote discovery...\r\n"));
			Bluetooth_Command_WriteScanEnable(InquiryAndPageScans);

			puts_P(PSTR("Waiting for events...\r\n"));

			Scheduler_SetTaskMode(Bluetooth_ProcessData, TASK_RUN);
			Scheduler_SetTaskMode(Bluetooth_ProcessEvents, TASK_RUN);
	
			/* Indicate device no longer busy */
			LEDs_SetAllLEDs(LEDS_LED4);
			
			break;
	}
}

TASK(Bluetooth_ProcessData)
{
	Bluetooth_ACL_Header_t ACLPacketHeader;

	Pipe_SelectPipe(BLUETOOTH_DATA_IN_PIPE);
	Pipe_SetToken(PIPE_TOKEN_IN);
	Pipe_Unfreeze();
	
	if (!(Pipe_ReadWriteAllowed()))
	{
		Pipe_Freeze();
		return;
	}
	  
	Pipe_Read_Stream_LE(&ACLPacketHeader, sizeof(ACLPacketHeader));
	
	puts_P(PSTR("ACL Packet Received\r\n"));
	printf_P(PSTR(" -- Connection Handle: 0x%04X\r\n"), ACLPacketHeader.ConnectionHandle);
	printf_P(PSTR(" -- Data Length: 0x%04X\r\n"), ACLPacketHeader.DataLength);
	
	Bluetooth_DataPacket_Header_t DataHeader;	
	Pipe_Read_Stream_LE(&DataHeader, sizeof(DataHeader));

	printf_P(PSTR(" -- Channel ID: 0x%04X\r\n"), DataHeader.ChannelID);
	printf_P(PSTR(" -- Payload Length: 0x%04X\r\n"), DataHeader.PayloadLength);

	if (DataHeader.ChannelID == CHANNEL_SIGNALLING)
	{
		Bluetooth_SignalCommand_Header_t SignalCommandHeader;
		Pipe_Read_Stream_LE(&SignalCommandHeader, sizeof(SignalCommandHeader));

		switch (SignalCommandHeader.Code)
		{
			case 0x02: // L2CAP Connection Request
				puts_P(PSTR("L2CAP Connection Request\r\n"));
				
				Pipe_Discard_Stream(DataHeader.PayloadLength - sizeof(SignalCommandHeader));
				break;
			default:
				puts_P(PSTR("Unknown Signalling Command\r\n"));

				Pipe_Discard_Stream(DataHeader.PayloadLength - sizeof(SignalCommandHeader));				
				break;				
		}
	}
	else
	{
		uint8_t DataPayload[DataHeader.PayloadLength];
		Pipe_Read_Stream_LE(&DataPayload, sizeof(DataPayload));
	
		puts_P(PSTR(" -- Data Payload: "));

		for (uint16_t B = 0; B < sizeof(DataPayload); B++)
		  printf("0x%02X ", DataPayload[B]);
		
		puts_P(PSTR("\r\n"));	
	}
	
	Pipe_ClearCurrentBank();
	Pipe_Freeze();
}

TASK(Bluetooth_ProcessEvents)
{
	Bluetooth_HCIEvent_Header_t EventInfo;

	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);

	if (!(Pipe_ReadWriteAllowed()))
	  return;
	
	Pipe_Read_Stream_LE(&EventInfo, sizeof(EventInfo));
				
	if (EventInfo.EventCode == EVENT_CONNECTION_REQUEST)
	{
		Bluetooth_EventParams_ConnectionRequest_t ConnectionRequestParams;
		
		Pipe_Read_Stream_LE(&ConnectionRequestParams, sizeof(ConnectionRequestParams));
		Pipe_ClearCurrentBank();

		uint8_t* RemoteAddressOctets = (uint8_t*)&ConnectionRequestParams.RemoteAddress.Octets;
		printf_P(PSTR("Connection Request from device %02X:%02X:%02X:%02X:%02X:%02X (%s)\r\n"), 
				 RemoteAddressOctets[5], RemoteAddressOctets[4], RemoteAddressOctets[3],
				 RemoteAddressOctets[2], RemoteAddressOctets[1], RemoteAddressOctets[0]);
				 
		printf_P(PSTR(" -- Device Class: 0x%02X%04X\r\n"), ConnectionRequestParams.ClassOfDevice_Service,
		                                                   ConnectionRequestParams.ClassOfDevice_MajorMinor);
		printf_P(PSTR(" -- Link Type: 0x%02x\r\n"), ConnectionRequestParams.LinkType);

		Bluetooth_Command_AcceptConnectionRequest(&ConnectionRequestParams.RemoteAddress, true);
	}
	else if (EventInfo.EventCode == EVENT_CONNECTION_COMPLETE)
	{
		Bluetooth_EventParams_ConnectionComplete_t ConnectionCompleteParams;
		
		Pipe_Read_Stream_LE(&ConnectionCompleteParams, sizeof(ConnectionCompleteParams));
		Pipe_ClearCurrentBank();
		
		uint8_t* RemoteAddressOctets = (uint8_t*)&ConnectionCompleteParams.RemoteAddress.Octets;
		printf_P(PSTR("Connection Complete to device %02X:%02X:%02X:%02X:%02X:%02X (%s)\r\n"), 
				 RemoteAddressOctets[5], RemoteAddressOctets[4], RemoteAddressOctets[3],
				 RemoteAddressOctets[2], RemoteAddressOctets[1], RemoteAddressOctets[0]);
				 
		printf_P(PSTR(" -- Connection Handle: 0x%04X\r\n"), ConnectionCompleteParams.ConnectionHandle);
		printf_P(PSTR(" -- Link Type: 0x%02X\r\n"), ConnectionCompleteParams.LinkType);
		printf_P(PSTR(" -- Encryption Type: 0x%02X\r\n"), ConnectionCompleteParams.EncryptionEnabled);
	}
	else if (EventInfo.EventCode == EVENT_DISCONNECTION_COMPLETE)
	{
		Bluetooth_EventParams_DisconnectionComplete_t DisconnectionCompleteParams;
		
		Pipe_Read_Stream_LE(&DisconnectionCompleteParams, sizeof(DisconnectionCompleteParams));
		Pipe_ClearCurrentBank();
		
		puts_P(PSTR("Connection disconnected.\r\n"));					
		printf_P(PSTR(" -- Connection Handle: 0x%04X\r\n"), DisconnectionCompleteParams.ConnectionHandle);
		printf_P(PSTR(" -- Reason: 0x%02X\r\n"), DisconnectionCompleteParams.Reason);					
	}
	else
	{
		printf_P(PSTR("UNK EVENT: Code 0x%02X, Param Length: %d\r\n"), EventInfo.EventCode, EventInfo.ParameterLength);

		Pipe_Discard_Stream(EventInfo.ParameterLength);
		Pipe_ClearCurrentBank();
	}
}

/** Function to manage status updates to the user. This is done via LEDs on the given board, if available, but may be changed to
 *  log to a serial port, or anything else that is suitable for status updates.
 *
 *  \param CurrentStatus  Current status of the system, from the MouseHostViaInt_StatusCodes_t enum
 */
void UpdateStatus(uint8_t CurrentStatus)
{
	uint8_t LEDMask = LEDS_NO_LEDS;
	
	/* Set the LED mask to the appropriate LED mask based on the given status code */
	switch (CurrentStatus)
	{
		case Status_USBNotReady:
			LEDMask = (LEDS_LED1);
			break;
		case Status_USBEnumerating:
			LEDMask = (LEDS_LED1 | LEDS_LED2);
			break;
		case Status_USBReady:
			LEDMask = (LEDS_LED2);
			break;
		case Status_EnumerationError:
		case Status_HardwareError:
			LEDMask = (LEDS_LED1 | LEDS_LED3);
			break;
	}
	
	/* Set the board LEDs to the new LED mask */
	LEDs_SetAllLEDs(LEDMask);
}
