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
	Dual Communications Device Class demonstration application.
	This gives a simple reference application for implementing
	a compound device with dual CDC functions acting as a pair
	of virtual serial ports. This demo uses Interface Association
	Descriptors to link together the pair of related CDC
	descriptors for each virtual serial port, which may not be
	supported in all OSes - Windows Vista is supported, as is
	XP (although the latter may need a hotfix to function).
	
	Joystick actions are transmitted to the host as strings
	through the first serial port. The device does not respond to
	serial data sent from the host in the first serial port.
	
	The second serial port echoes back data sent from the host.
	
	Before running, you will need to install the INF file that
	is located in the DualCDC project directory. This will enable
	Windows to use its inbuilt CDC drivers, negating the need
	for special Windows drivers for the device.
*/

/*
	USB Mode:           Device
	USB Class:          Communications Device Class (CDC)
	USB Subclass:       Abstract Control Model (ACM)
	Relevant Standards: USBIF CDC Class Standard
	                    Interface Association Descriptor ECN
	Usable Speeds:      Full Speed Mode
*/

#include "DualCDC.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB DualCDC App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: CDC1_Task            , TaskStatus: TASK_STOP },
	{ Task: CDC2_Task            , TaskStatus: TASK_STOP },
};

/* Globals: */
CDC_Line_Coding_t LineCoding1 = { BaudRateBPS: 9600,
                                  CharFormat:  OneStopBit,
                                  ParityType:  Parity_None,
                                  DataBits:    8            };

CDC_Line_Coding_t LineCoding2 = { BaudRateBPS: 9600,
                                  CharFormat:  OneStopBit,
                                  ParityType:  Parity_None,
                                  DataBits:    8            };
								  
char JoystickUpString[]      = "Joystick Up\r\n";
char JoystickDownString[]    = "Joystick Down\r\n";
char JoystickLeftString[]    = "Joystick Left\r\n";
char JoystickRightString[]   = "Joystick Right\r\n";
char JoystickPressedString[] = "Joystick Pressed\r\n";

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
	/* Stop running CDC and USB management tasks */
	Scheduler_SetTaskMode(CDC1_Task, TASK_STOP);
	Scheduler_SetTaskMode(CDC2_Task, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup CDC Notification, Rx and Tx Endpoints for the first CDC */
	Endpoint_ConfigureEndpoint(CDC1_NOTIFICATION_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC1_TX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC1_RX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Setup CDC Notification, Rx and Tx Endpoints for the second CDC */
	Endpoint_ConfigureEndpoint(CDC2_NOTIFICATION_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC2_TX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC2_RX_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);
							   
	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
	
	/* Start CDC tasks */
	Scheduler_SetTaskMode(CDC1_Task, TASK_RUN);
	Scheduler_SetTaskMode(CDC2_Task, TASK_RUN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	uint8_t* LineCodingData;

	/* Discard the unused wValue parameter */
	Endpoint_Ignore_Word();

	/* wIndex indicates the interface being controlled */
	uint16_t wIndex = Endpoint_Read_Word_LE();

	/* Determine which interface's Line Coding data is being set from the wIndex parameter */
	LineCodingData = (wIndex == 0) ? (uint8_t*)&LineCoding1 : (uint8_t*)&LineCoding2;

	/* Process CDC specific control requests */
	switch (bRequest)
	{
		case GET_LINE_CODING:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{	
				/* Acknowedge the SETUP packet, ready for data transfer */
				Endpoint_ClearSetupReceived();

				/* Write the line coding data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(LineCodingData, sizeof(CDC_Line_Coding_t));
				
				/* Send the line coding data to the host and clear the control endpoint */
				Endpoint_ClearSetupOUT();
			}
			
			break;
		case SET_LINE_CODING:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Acknowedge the SETUP packet, ready for data transfer */
				Endpoint_ClearSetupReceived();

				/* Read the line coding data in from the host into the global struct */
				Endpoint_Read_Control_Stream_LE(LineCodingData, sizeof(CDC_Line_Coding_t));

				/* Send the line coding data to the host and clear the control endpoint */
				Endpoint_ClearSetupIN();
			}
	
			break;
		case SET_CONTROL_LINE_STATE:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Acknowedge the SETUP packet, ready for data transfer */
				Endpoint_ClearSetupReceived();
				
				/* Send an empty packet to acknowedge the command (currently unused) */
				Endpoint_ClearSetupIN();
			}
	
			break;
	}
}

TASK(CDC1_Task)
{
	char*       ReportString    = NULL;
	uint8_t     JoyStatus_LCL   = Joystick_GetStatus();
	static bool ActionSent      = false;

	/* Determine if a joystick action has occurred */
	if (JoyStatus_LCL & JOY_UP)
	  ReportString = JoystickUpString;
	else if (JoyStatus_LCL & JOY_DOWN)
	  ReportString = JoystickDownString;
	else if (JoyStatus_LCL & JOY_LEFT)
	  ReportString = JoystickLeftString;
	else if (JoyStatus_LCL & JOY_RIGHT)
	  ReportString = JoystickRightString;
	else if (JoyStatus_LCL & JOY_PRESS)
	  ReportString = JoystickPressedString;

	/* Flag management - Only allow one string to be sent per action */
	if (ReportString == NULL)
	{
		ActionSent = false;
	}
	else if (ActionSent == false)
	{
		ActionSent = true;
		
		/* Select the Serial Tx Endpoint */
		Endpoint_SelectEndpoint(CDC1_TX_EPNUM);

		/* Write the String to the Endpoint */
		Endpoint_Write_Stream_LE(ReportString, strlen(ReportString));
		
		/* Send the data */
		Endpoint_ClearCurrentBank();
	}

	/* Select the Serial Rx Endpoint */
	Endpoint_SelectEndpoint(CDC1_RX_EPNUM);
	
	/* Throw away any recieved data from the host */
	if (Endpoint_ReadWriteAllowed())
	  Endpoint_ClearCurrentBank();
}

TASK(CDC2_Task)
{
	/* Select the Serial Rx Endpoint */
	Endpoint_SelectEndpoint(CDC2_RX_EPNUM);
	
	/* Check to see if any data has been received */
	if (Endpoint_ReadWriteAllowed())
	{
		/* Create a temp buffer big enough to hold the incomming endpoint packet */
		uint8_t  Buffer[Endpoint_BytesInEndpoint()];
		
		/* Remember how large the incomming packet is */
		uint16_t DataLength = Endpoint_BytesInEndpoint();
	
		/* Read in the incomming packet into the buffer */
		Endpoint_Read_Stream_LE(&Buffer, DataLength);

		/* Clear the data from the reception endpoint */
		Endpoint_ClearCurrentBank();

		/* Select the Serial Tx Endpoint */
		Endpoint_SelectEndpoint(CDC2_TX_EPNUM);
		
		/* Write the received data to the endpoint */
		Endpoint_Write_Stream_LE(&Buffer, DataLength);

		/* Send the endpoint data back to the host */
		Endpoint_ClearCurrentBank();
	}
}
