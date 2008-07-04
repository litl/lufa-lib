/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Remote Network Driver Interface demonstration application.
	This gives a simple reference application for implementing
	a CDC RNDIS device acting as a simple network interface for
	ethernet packet exchange. RNDIS is a proprietary Microsoft
	standard; this demo will only work on Win2000 (patched with
	a RNDIS hotfix) and above, or on the latest Linux kernels.
	
	Before running, you will need to install the INF file that
	is located in the RNDISEthernet project directory. This will
	enable Windows to use its inbuilt RNDIS drivers, negating the
	need for special Windows drivers for the device. To install,
	right-click the .INF file and choose the Install option.

	When enumerated, this demo will install as a new network
	adapter which ethernet packets can be sent to and recieved
	from. Incomming packet information is printed through the
	AVR's serial port.
*/

/*
	USB Mode:           Device
	USB Class:          Communications Device Class (CDC)
	USB Subclass:       Remote NDIS (Microsoft Proprietary CDC Class Networking Standard)
	Relevant Standards: Microsoft RNDIS Specification
	Usable Speeds:      Full Speed Mode
*/

#include "RNDISEthernet.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB RNDIS App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: RNDIS_Task           , TaskStatus: TASK_STOP },
	{ Task: Ethernet_Task        , TaskStatus: TASK_STOP },
};

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	Joystick_Init();
	LEDs_Init();
	SerialStream_Init(9600);
	
	printf("\r\n\r\n****** RNDIS Demo running. ******\r\n");

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
	
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_Connect)
{
	/* Start USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);

	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED4);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running RNDIS, Ethernet and USB management tasks */
	Scheduler_SetTaskMode(RNDIS_Task, TASK_STOP);
	Scheduler_SetTaskMode(Ethernet_Task, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup CDC Notification, Rx and Tx Endpoints */
	Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);

	/* Start RNDIS and Ethernet tasks */
	Scheduler_SetTaskMode(RNDIS_Task, TASK_RUN);
	Scheduler_SetTaskMode(Ethernet_Task, TASK_RUN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Discard the unused wValue parameter */
	Endpoint_Discard_Word();

	/* Discard the unused wIndex parameter */
	Endpoint_Discard_Word();

	/* Read in the wLength parameter */
	uint16_t wLength = Endpoint_Read_Word_LE();

	/* Process RNDIS class commands */
	switch (bRequest)
	{
		case SEND_ENCAPSULATED_COMMAND:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Clear the SETUP packet, ready for data transfer */
				Endpoint_ClearSetupReceived();
				
				/* Read in the RNDIS message into the message buffer */
				Endpoint_Read_Control_Stream_LE(RNDISBuffer, wLength);

				/* Clear the endpoint, ready for next control request */
				Endpoint_ClearSetupIN();

				/* Process the RNDIS message */
				ProcessRNDISControlMessage();
			}
			
			break;
		case GET_ENCAPSULATED_RESPONSE:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Check if less than the requested number of bytes to transfer */
				if (MessageHeader->MessageLength < wLength)
				  wLength = MessageHeader->MessageLength;

				/* Clear the SETUP packet, ready for data transfer */
				Endpoint_ClearSetupReceived();
							
				/* Check if a response to the last message is ready */
				if (MessageHeader->MessageLength)
				{
					/* Write the message response data to the endpoint */
					Endpoint_Write_Control_Stream_LE(RNDISBuffer, wLength);
					
					/* Reset the message header once again after transmission */
					MessageHeader->MessageLength = 0;
				}
				else
				{
					/* RNDIS specifies a single 0x00 to indicate that no response is ready */
					Endpoint_Write_Byte(0x00);
				}
				
				/* Send the endpoint data and clear the endpoint ready for the next command */
				Endpoint_ClearSetupOUT();
			}
	
			break;
	}
}

TASK(RNDIS_Task)
{
	/* Check if a message response is ready for the host */
	if (ResponseReady)
	{
		USB_Notification_t Notification = (USB_Notification_t)
			{
				bmRequestType: (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
				bNotification: RESPONSE_AVAILABLE,
				wValue:        0,
				wIndex:        0,
				wLength:       0,
			};

		/* Select the notification endpoint */
		Endpoint_SelectEndpoint(CDC_NOTIFICATION_EPNUM);
		
		/* Indicate that a message response is ready for the host */
		Endpoint_Write_Stream_LE(&Notification, sizeof(Notification));

		/* Send the notification to the host */
		Endpoint_ClearCurrentBank();

		ResponseReady = false;
	}
	
	/* Don't process the data endpoints until the system is in the data initialized state, and the buffer is free */
	if ((CurrRNDISState == RNDIS_Data_Initialized) && !(MessageHeader->MessageLength))
	{
		Endpoint_SelectEndpoint(CDC_RX_EPNUM);
		
		if (Endpoint_ReadWriteAllowed() && !(IsFrameIN))
		{
			/* Read in the packet message header */
			Endpoint_Read_Stream_LE(RNDISBuffer, sizeof(RNDIS_PACKET_MSG_t));
			
			/* Create a packet message pointer to the read in data */
			RNDIS_PACKET_MSG_t* RNDISPacketHeader = (RNDIS_PACKET_MSG_t*)&RNDISBuffer;
	
			/* Read in the Ethernet frame into the buffer */
			Endpoint_Read_Stream_LE(EthernetFrameIN, (RNDISPacketHeader->MessageLength -
			                                          sizeof(RNDIS_PACKET_MSG_t)));		

			/* Clear the endpoint bank ready for next packet */
			Endpoint_ClearCurrentBank();
			
			/* Store the size of the Ethernet frame */
			EthernetFrameINLength = RNDISPacketHeader->DataLength;

			/* Indicate Ethernet IN buffer full */
			IsFrameIN = true;
		}
		
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);
		
		if (Endpoint_ReadWriteAllowed() && IsFrameOUT)
		{
			/* Create a packet message pointer to the buffer */
			RNDIS_PACKET_MSG_t* RNDISPacketHeader = (RNDIS_PACKET_MSG_t*)&RNDISBuffer;
			
			/* Construct the packet header into the buffer */
			RNDISPacketHeader->MessageType         = REMOTE_NDIS_PACKET_MSG;
			RNDISPacketHeader->MessageLength       = (sizeof(RNDIS_PACKET_MSG_t) + EthernetFrameOUTLength);
			RNDISPacketHeader->DataOffset          = (sizeof(RNDIS_PACKET_MSG_t) - sizeof(RNDIS_Message_Header_t));
			RNDISPacketHeader->DataLength          = EthernetFrameOUTLength;
			RNDISPacketHeader->OOBDataOffset       = 0;
			RNDISPacketHeader->OOBDataLength       = 0;
			RNDISPacketHeader->NumOOBDataElements  = 0;
			RNDISPacketHeader->PerPacketInfoOffset = 0;
			RNDISPacketHeader->PerPacketInfoLength = 0;
			RNDISPacketHeader->VcHandle            = 0;
			RNDISPacketHeader->Reserved            = 0;
			
			/* Send the packet header to the host */
			Endpoint_Write_Stream_LE(RNDISPacketHeader, sizeof(RNDISPacketHeader));
			
			/* Send the Ethernet frame data to the host */
			Endpoint_Write_Stream_LE(EthernetFrameOUT, EthernetFrameOUTLength);

			/* Clear the endpoint bank ready for the next packet */
			Endpoint_ClearCurrentBank();
			
			/* Indicate Ethernet OUT buffer no longer full */
			IsFrameOUT = false;
		}
	}
}

TASK(Ethernet_Task)
{
	/* Task for Ethernet processing. An incomming Ethernet frames is available in EthernetFrameIN[] when
	   IsFrameIN is set, with the frame length stored in EthernetFrameINLength. Outgoing frames should be
	   placed in EthernetFrameOUT[] with their length in EthernetFrameOUTLength, and IsFrameOUT set high. */

	/* Check if a frame has been written to the IN frame buffer */
	if (IsFrameIN)
	{
		/* Print out the frame details */
		Ethernet_ProcessPacket();

		/* Clear the frame buffer */
		IsFrameIN = false;
	}
}
