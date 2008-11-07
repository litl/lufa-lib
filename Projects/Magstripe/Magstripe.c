/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Denver Gingerich (denver [at] ossguy [dot] com)

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

/** \file
 *
 *  Main source file for the MagStripe application. This file contains the code which drives
 *  the USB keyboard interface from the magnetic card stripe reader device.
 */
 
#include "Magstripe.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "Magstripe Reader");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Keyboard_Report  , TaskStatus: TASK_STOP },
};

/* Global Variables */
/** Indicates if the device is using Report Protocol mode, instead of Boot Protocol mode. Boot Protocol mode
 *  is a special reporting mode used by compatible PC BIOS to support USB keyboards before a full OS and USB
 *  driver has been loaded, by using predefined report structures indicated in the USB HID standard.
 */
bool UsingReportProtocol = true;

/** Total idle period in milliseconds set by the host via a SetIdle request, used to silence the report endpoint
 *  until the report data changes or the idle period elapsed. Generally used to implement hardware key repeats, or
 *  by some BIOS to reduce the number of reports when in Boot Protocol mode.
 */
uint8_t IdleCount = 0;

/** Milliseconds remaining counter for the HID class SetIdle and GetIdle requests, used to silence the report
 *  endpoint for an amount of time indicated by the host or until the report changes.
 */
uint16_t IdleMSRemaining = 0;


/** Main program entry point. This routine configures the hardware required by the application, then
 *  starts the scheduler to run the application tasks.
 */
int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	Magstripe_Init();
	LEDs_Init();
	
	/* Millisecond timer initialization, with output compare interrupt enabled for the idle timing */
	OCR0A  = 0x7D;
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS01) | (1 << CS00));
	TIMSK0 = (1 << OCIE0A);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
	
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();
	
	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

/** Event handler for the USB_Connect event. This starts the USB task. */
EVENT_HANDLER(USB_Connect)
{
	/* Start USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);

	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED4);
}

/** Event handler for the USB_Disconnect event. This stops the USB and keyboard report tasks. */
EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running keyboard reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_Keyboard_Report, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

/** Event handler for the USB_ConfigurationChanged event. This configures the device's endpoints ready
 *  to relay reports to the host, and starts the keyboard report task.
 */
EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Keyboard Keycode Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Setup Keyboard LED Report Endpoint */
	Endpoint_ConfigureEndpoint(KEYBOARD_LEDS_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_OUT, KEYBOARD_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
	
	/* Default to report protocol on connect */
	UsingReportProtocol = true;

	/* Start Keyboard reporting task */
	Scheduler_SetTaskMode(USB_Keyboard_Report, TASK_RUN);
}

/** Event handler for the USB_UnhandledControlPacket event. This is used to catch standard and class specific
 *  control requests that are not handled internally by the USB library, so that they can be handled appropriately
 *  for the application.
 */
EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Handle HID Class specific requests */
	switch (bRequest)
	{
		case REQ_GetReport:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				USB_KeyboardReport_Data_t KeyboardReportData;

				/* Create the next keyboard report for transmission to the host */
				GetNextReport(&KeyboardReportData);

				/* Ignore report type and ID number value */
				Endpoint_Discard_Word();
				
				/* Ignore unused Interface number value */
				Endpoint_Discard_Word();

				/* Read in the number of bytes in the report to send to the host */
				uint16_t wLength = Endpoint_Read_Word_LE();
				
				/* If trying to send more bytes than exist to the host, clamp the value at the report size */
				if (wLength > sizeof(KeyboardReportData))
				  wLength = sizeof(KeyboardReportData);

				Endpoint_ClearSetupReceived();
	
				/* Write the report data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(&KeyboardReportData, wLength);
				
				/* Finalize the transfer, acknowedge the host error or success OUT transfer */
				Endpoint_ClearSetupOUT();
			}
		
			break;
		case REQ_SetReport:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				
				/* Wait until the LED report has been sent by the host */
				while (!(Endpoint_IsSetupOUTReceived()));

				/* Read in the LED report from the host */
				uint8_t LEDStatus = Endpoint_Read_Byte();

				/* Process the incomming LED report */
				ProcessLEDReport(LEDStatus);
			
				/* Clear the endpoint data */
				Endpoint_ClearSetupOUT();

				/* Wait until the host is ready to receive the request confirmation */
				while (!(Endpoint_IsSetupINReady()));
				
				/* Handshake the request by sending an empty IN packet */
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_GetProtocol:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				
				/* Write the current protocol flag to the host */
				Endpoint_Write_Byte(UsingReportProtocol);
				
				/* Send the flag to the host */
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_SetProtocol:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Read in the wValue parameter containing the new protocol mode */
				uint16_t wValue = Endpoint_Read_Word_LE();
				
				/* Set or clear the flag depending on what the host indicates that the current Protocol should be */
				UsingReportProtocol = (wValue != 0x0000);
				
				Endpoint_ClearSetupReceived();

				/* Send an empty packet to acknowedge the command */
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_SetIdle:
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				/* Read in the wValue parameter containing the idle period */
				uint16_t wValue = Endpoint_Read_Word_LE();
				
				Endpoint_ClearSetupReceived();
				
				/* Get idle period in MSB */
				IdleCount = (wValue >> 8);
				
				/* Send an empty packet to acknowedge the command */
				Endpoint_ClearSetupIN();
			}
			
			break;
		case REQ_GetIdle:
			if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{		
				Endpoint_ClearSetupReceived();
				
				/* Write the current idle duration to the host */
				Endpoint_Write_Byte(IdleCount);
				
				/* Send the flag to the host */
				Endpoint_ClearSetupIN();
			}

			break;
	}
}

/** ISR for the timer 0 compare vector. This ISR fires once each millisecond, and decrements the counter indicating
 *  the number of milliseconds left to idle (not send the host reports) if the device has been instructed to idle
 *  by the host via a SetIdle class specific request.
 */
ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	/* One millisecond has elapsed, decrement the idle time remaining counter if it has not already elapsed */
	if (IdleMSRemaining)
	  IdleMSRemaining--;
}

/** Constructs a keyboard report indicating the currently pressed keyboard keys to the host.
 *
 *  \param ReportData  Pointer to a USB_KeyboardReport_Data_t report structure where the resulting report should
 *                     be stored
 *
 *  \return Boolean true if the current report is different to the previous report, false otherwise
 */
bool GetNextReport(USB_KeyboardReport_Data_t* ReportData)
{
	static uint8_t PrevMagStatus = 0;
	uint8_t        MagStatus_LCL = Magstripe_GetStatus();
	bool           InputChanged  = false;

	/* Clear the report contents */
	memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));
	  
	/* Check if the new report is different to the previous report */
	InputChanged = PrevMagStatus ^ MagStatus_LCL;

	/* Save the current magstripe status for later comparison */
	PrevMagStatus = MagStatus_LCL;

	/* Return whether the new report is different to the previous report or not */
	return InputChanged;
}

/** Processes a LED status report from the host to the device, and displays the current LED status (Caps Lock,
 *  Num Lock and Scroll Lock) onto the board LEDs.
 *
 *  \param LEDReport  Report from the host indicating the current keyboard LED status
 */
void ProcessLEDReport(uint8_t LEDReport)
{
	uint8_t LEDMask   = LEDS_LED2;
	
	if (LEDReport & 0x01) // NUM Lock
	  LEDMask |= LEDS_LED1;
	
	if (LEDReport & 0x02) // CAPS Lock
	  LEDMask |= LEDS_LED3;

	if (LEDReport & 0x04) // SCROLL Lock
	  LEDMask |= LEDS_LED4;

	/* Set the status LEDs to the current Keyboard LED status */
	LEDs_SetAllLEDs(LEDMask);
}

/** Task for the magnetic card reading and keyboard report generation. This task waits until a card is inserted,
 *  then reads off the card data and sends it to the host as a series of keyboard keypresses via keyboard reports.
 */
TASK(USB_Keyboard_Report)
{
	USB_KeyboardReport_Data_t KeyboardReportData;
	bool                      SendReport;
	
	uint8_t MagStatus_LCL = Magstripe_GetStatus();

	bool T1_StrobeFired = false;
	uint8_t T1_Bits[T1_MAX_BITS];
	uint16_t T1_NumBits = 0;

	bool T2_StrobeFired = false;
	uint8_t T2_Bits[T2_MAX_BITS];
	uint16_t T2_NumBits = 0;

	bool T3_StrobeFired = false;
	uint8_t T3_Bits[T3_MAX_BITS];
	uint16_t T3_NumBits = 0;

	/* Create the next keyboard report for transmission to the host */
	SendReport = GetNextReport(&KeyboardReportData);
	Send(&KeyboardReportData, SendReport);
	_delay_ms(2);

	/* Check if a card is present, abort if no card inserted into the reader */
	if (!(MagStatus_LCL & MAG_CLS))
	  return;

	while (MagStatus_LCL & MAG_CLS)
	{
		/* get current magstripe reader pin state */
		MagStatus_LCL = Magstripe_GetStatus();

		/* Track 1 */
		if ( (MagStatus_LCL & MAG_T1_CLOCK) /* if t1_strobe low */
			 && !T1_StrobeFired ) {         /*  and !t1_strobe_fired */
				if (MagStatus_LCL & MAG_T1_DATA) { /* if t1_data low */
				  T1_Bits[T1_NumBits] = KEY_1;
				} else {
				  T1_Bits[T1_NumBits] = KEY_0;
				}

				T1_NumBits = (T1_NumBits + 1) % T1_MAX_BITS;
				T1_StrobeFired = true;

		} else if ( !(MagStatus_LCL & MAG_T1_CLOCK) ) {
				/* if t1_strobe high */
				T1_StrobeFired = false;
		}

		/* Track 2 */
		if ( (MagStatus_LCL & MAG_T2_CLOCK) /* if t2_strobe low */
			 && !T2_StrobeFired ) {         /*  and !t2_strobe_fired */
				if (MagStatus_LCL & MAG_T2_DATA) { /* if t2_data low */
				  T2_Bits[T2_NumBits] = KEY_1;
				} else {
				  T2_Bits[T2_NumBits] = KEY_0;
				}

				T2_NumBits = (T2_NumBits + 1) % T2_MAX_BITS;
				T2_StrobeFired = true;

		} else if ( !(MagStatus_LCL & MAG_T2_CLOCK) ) {
				/* if t2_strobe high */
				T2_StrobeFired = false;
		}

		/* Track 3 */
		if ( (MagStatus_LCL & MAG_T3_CLOCK) /* if t3_strobe low */
			 && !T3_StrobeFired ) {         /*  and !t3_strobe_fired */
				if (MagStatus_LCL & MAG_T3_DATA) { /* if t3_data low */
				  T3_Bits[T3_NumBits] = KEY_1;
				} else {
				  T3_Bits[T3_NumBits] = KEY_0;
				}

				T3_NumBits = (T3_NumBits + 1) % T3_MAX_BITS;
				T3_StrobeFired = true;

		} else if ( !(MagStatus_LCL & MAG_T3_CLOCK) ) {
				/* if t1_strobe high */
				T3_StrobeFired = false;
		}
	}

	/* type Track 1 */
	for (uint16_t i = 0; i < T1_NumBits; i++)
	{
		SendKey(&KeyboardReportData, T1_Bits[i]);

		/* send "no event" key; this is required so that the OS does
		 * not ignore multiple keypresses of the same key (which is
		 * what occurs when you hold down a key - the OS has a repeat
		 * delay to prevent it from immediately repeating the key)
		 */
		SendKey(&KeyboardReportData, KEY_NO_EVENT);
	}

	SendKey(&KeyboardReportData, KEY_ENTER);
	SendKey(&KeyboardReportData, KEY_NO_EVENT);

	/* type Track 2 */
	for (uint16_t i = 0; i < T2_NumBits; i++)
	{
		SendKey(&KeyboardReportData, T2_Bits[i]);
		SendKey(&KeyboardReportData, KEY_NO_EVENT);
	}

	SendKey(&KeyboardReportData, KEY_ENTER);
	SendKey(&KeyboardReportData, KEY_NO_EVENT);

	/* type Track 3 */
	for (uint16_t i = 0; i < T3_NumBits; i++)
	{
		SendKey(&KeyboardReportData, T3_Bits[i]);
		SendKey(&KeyboardReportData, KEY_NO_EVENT);
	}

	SendKey(&KeyboardReportData, KEY_ENTER);
	SendKey(&KeyboardReportData, KEY_NO_EVENT);
}

/** Creates a keypress report from the given key code and sends the report to the host.
 *
 *  \param KeyboardReportData  Pointer to a USB_KeyboardReport_Data_t structure where the key report can be stored
 *  \param Key  Key code of the key to send
 */
void SendKey(USB_KeyboardReport_Data_t* KeyboardReportData, uint8_t Key)
{
	memset(KeyboardReportData, 0, sizeof(USB_KeyboardReport_Data_t));
	KeyboardReportData->KeyCode[0] = Key;
	Send(KeyboardReportData, true);

	/* a delay of at least 2ms is required between key events; if a key
	 * event X happens less than 2ms after key event Y, then key event X
	 * will be ignored
	 */
	_delay_ms(2);
}	

/** Sends the given keyboard report to the host, if required.
 *
 *  \param KeyboardReportData  Pointer to the keyboard report to send
 *  \param SendReport  Boolean true if the report should be sent, false otherwise
 */
void Send(USB_KeyboardReport_Data_t* KeyboardReportData, bool SendReport)
{
	/* Check if the idle period is set and has elapsed */
	if (IdleCount && !(IdleMSRemaining))
	{
		/* Idle period elapsed, indicate that a report must be sent */
		SendReport = true;
		
		/* Reset the idle time remaining counter, must multiply by 4 to get the duration in milliseconds */
		IdleMSRemaining = (IdleCount << 2);
	}
	
	/* Check if the USB system is connected to a host */
	if (USB_IsConnected)
	{
		/* Select the Keyboard Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_EPNUM);

		/* Check if Keyboard Endpoint Ready for Read/Write, and if we should send a report */
		if (Endpoint_ReadWriteAllowed() && SendReport)
		{
			/* Write Keyboard Report Data */
			Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(USB_KeyboardReport_Data_t));

			/* Handshake the IN Endpoint - send the data to the host */
			Endpoint_ClearCurrentBank();
		}

		/* Select the Keyboard LED Report Endpoint */
		Endpoint_SelectEndpoint(KEYBOARD_LEDS_EPNUM);

		/* Check if Keyboard LED Endpoint Ready for Read/Write */
		if (Endpoint_ReadWriteAllowed())
		{
			/* Read in the LED report from the host */
			uint8_t LEDStatus = Endpoint_Read_Byte();

			/* Process the incomming LED report */
			ProcessLEDReport(LEDStatus);

			/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
			Endpoint_ClearCurrentBank();
		}
	}
}
