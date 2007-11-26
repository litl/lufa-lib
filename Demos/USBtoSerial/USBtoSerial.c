/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Communications Device Class demonstration application.
	This gives a simple reference application for implementing
	a USB to Serial converter. Sent and recieved data on the
	serial port is communicated to the USB host.
	
	Before running, you will need to install the INF file that
	is located in the CDC project directory. This will enable
	Windows to use its inbuilt CDC drivers, negating the need
	for special Windows drivers for the device. To install,
	right-click the .INF file and choose the Install option.
*/

#include "USBtoSerial.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB USB RS232 App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	CDC_Task_ID
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: CDC_Task_ID             , TaskName: CDC_Task             , TaskStatus: TASK_RUN  },
};

/* Globals: */
CDC_Line_Coding_t LineCoding = { BaudRateBPS: 9600,
                                 CharFormat:  OneStopBit,
                                 ParityType:  Parity_None,
                                 DataBits:    8            };

RingBuff_t Rx_Buffer;
RingBuff_t Tx_Buffer;

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Bicolour_Init();
	ReconfigureUSART();
	
	/* Ringbuffer Initialization */
	Buffer_Initialize(&Rx_Buffer);
	Buffer_Initialize(&Tx_Buffer);
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup CDC Notification, Rx and Tx Endpoints */
	Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM, ENDPOINT_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, ENDPOINT_TYPE_BULK,
		                       ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, ENDPOINT_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Process CDC specific control requests */
	uint8_t* LineCodingData = (uint8_t*)&LineCoding;

	USB_Device_Ignore_Word();

	switch (Request)
	{
		case GET_LINE_CODING:
			Endpoint_ClearSetupRecieved();

			for (uint8_t i = 0; i < sizeof(LineCoding); i++)
			  USB_Device_Write_Byte(*(LineCodingData++));	
			
			Endpoint_In_Clear();
			while (!(Endpoint_In_IsReady()));
			
			while (!(Endpoint_Out_IsRecieved()));
			Endpoint_Out_Clear();

			break;
		case SET_LINE_CODING:
			Endpoint_ClearSetupRecieved();
			
			while (!(Endpoint_Out_IsRecieved()));

			for (uint8_t i = 0; i < sizeof(LineCoding); i++)
			  *(LineCodingData++) = USB_Device_Read_Byte();

			Endpoint_Out_Clear();

			Endpoint_In_Clear();
			while (!(Endpoint_In_IsReady()));
						
			ReconfigureUSART();
	
			break;
		case SET_CONTROL_LINE_STATE:
			Endpoint_ClearSetupRecieved();
			
			Endpoint_In_Clear();
			while (!(Endpoint_In_IsReady()));
	
			break;
	}
}

TASK(CDC_Task)
{
	/* Select the Serial Rx Endpoint */
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	
	if (Endpoint_Out_IsRecieved())
	{
		/* Read the recieved data endpoint into the transmission buffer */
		while (Endpoint_BytesInEndpoint())
		  Buffer_StoreElement(&Rx_Buffer, USB_Device_Read_Byte());
		
		/* Clear the endpoint buffer */
		Endpoint_Out_Clear();

		/* Initiate the transmission of the buffer contents if USART idle */
		if (UCSR1A & (1 << UDRE1))
		{
			UCSR1A &= ~(1 << UDRE1);
			Serial_TxByte(Buffer_GetElement(&Rx_Buffer));
		}
	}

	/* Select the Serial Tx Endpoint */
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);

	if (Tx_Buffer.Elements)
	{
		/* Wait until Serial Tx Endpoint Ready for Read/Write */
		while (!(Endpoint_ReadWriteAllowed()));
		
		/* Write the transmission buffer contents to the recieved data endpoint */
		while (Tx_Buffer.Elements)
		  USB_Device_Write_Byte(Buffer_GetElement(&Tx_Buffer));
	  
		/* Send the data */
		Endpoint_In_Clear();	
	}
}

ISR(USART1_TX_vect)
{
	/* Send next character if avaliable */
	if (Rx_Buffer.Elements)
	  UDR1 = Buffer_GetElement(&Rx_Buffer);
}

ISR(USART1_RX_vect)
{
	/* Character recieved, store it into the buffer */
	Buffer_StoreElement(&Tx_Buffer, UDR1);
}

void ReconfigureUSART(void)
{
	uint8_t ConfigMask = 0;

	/* Determine parity - non odd/even parity mode defaults to no parity */
	if (LineCoding.ParityType == Parity_Odd)
	  ConfigMask = ((1 << UPM11) | (1 << UPM10));
	else if (LineCoding.ParityType == Parity_Even)
	  ConfigMask = (1 << UPM11);

	/* Determine stop bits - 1.5 stop bits is set as 1 stop bit due to hardware limitations */
	if (LineCoding.CharFormat == TwoStopBits)
	  ConfigMask |= (1 << USBS1);

	/* Determine data size - 5, 6, 7, or 8 bits are supported */
	if (LineCoding.DataBits == 6)
	  ConfigMask |= (1 << UCSZ10);
	else if (LineCoding.DataBits == 7)
	  ConfigMask |= (1 << UCSZ11);
	else if (LineCoding.DataBits == 8)
	  ConfigMask |= ((1 << UCSZ11) | (1 << UCSZ10));
	
	/* Enable double speed, gives better error percentages at 8MHz */
	UCSR1A = (1 << U2X1);
	
	/* Enable transmit and recieve modules and interrupts */
	UCSR1B = ((1 << TXCIE1) | (1 << RXCIE1) | (1 << TXEN1) | (1 << RXEN1));

	/* Set the USART mode to the mask generated by the Line Coding options */
	UCSR1C = ConfigMask;
	
	/* Set the USART baud rate register to the desired baud rate value */
	UBRR1  = SERIAL_2X_UBBRVAL((uint16_t)LineCoding.BaudRateBPS);
}
