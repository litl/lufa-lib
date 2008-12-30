/*
             LUFA Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Denver Gingerich (denver [at] ossguy [dot] com)
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

/** \file
 *
 *  Main source file for the MagStripe application. This file contains the code which drives
 *  the USB keyboard interface from the magnetic card stripe reader device.
 */
 
#include "Magstripe.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,    "Magstripe Reader");
BUTTLOADTAG(BuildTime,   __TIME__);
BUTTLOADTAG(BuildDate,   __DATE__);
BUTTLOADTAG(LUFAVersion, "LUFA V" LUFA_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Keyboard_Report  , TaskStatus: TASK_STOP },
	{ Task: Magstripe_Read       , TaskStatus: TASK_STOP },
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

/** Circular buffer to hold the read bits from track 1 of the inserted magnetic card. */
RingBuff_t Track1Data;

/** Circular buffer to hold the read bits from track 2 of the inserted magnetic card. */
RingBuff_t Track2Data;

/** Circular buffer to hold the read bits from track 3 of the inserted magnetic card. */
RingBuff_t Track3Data;

/** Delay counter between sucessive key strokes. This is to prevent the OS from ignoring multiple keys in a short
 *  period of time due to key repeats. Two milliseconds works for most OSes.
 */
uint8_t KeyDelayRemaining;


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
	
	Buffer_Initialize(&Track1Data);
	Buffer_Initialize(&Track2Data);
	Buffer_Initialize(&Track3Data);
	
	/* Millisecond timer initialization, with output compare interrupt enabled for the idle timing */
	OCR0A  = 0xFA;
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS01) | (1 << CS00));
	TIMSK0 = (1 << OCIE0A);
	
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
}

/** Event handler for the USB_Disconnect event. This stops the USB and keyboard report tasks. */
EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running keyboard reporting, card reading and USB management tasks */
	Scheduler_SetTaskMode(USB_Keyboard_Report, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);
	Scheduler_SetTaskMode(Magstripe_Read, TASK_STOP);
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
	
	/* Default to report protocol on connect */
	UsingReportProtocol = true;

	/* Start Keyboard reporting and card reading tasks */
	Scheduler_SetTaskMode(USB_Keyboard_Report, TASK_RUN);
	Scheduler_SetTaskMode(Magstripe_Read, TASK_RUN);
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
	  
	if (KeyDelayRemaining)
	  KeyDelayRemaining--;
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
	static bool OddReport   = false;
	static bool MustRelease = false;

	/* Clear the report contents */
	memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

	if (Track1Data.Elements || Track2Data.Elements || Track3Data.Elements)
	{
		OddReport   = !OddReport;
		MustRelease = true;

		if (OddReport)
		{
			RingBuff_t* Buffer;
			
			if (Track1Data.Elements)
			  Buffer = &Track1Data;
			else if (Track2Data.Elements)
			  Buffer = &Track2Data;			
			else
			  Buffer = &Track3Data;
			
			ReportData->KeyCode[0] = Buffer_GetElement(Buffer);
		}

		return true;
	}
	else if (MustRelease)
	{
		return true;
	}
	
	return false;
}

/** Task to read out data from inserted magnetic cards and place the seperate track data into their respective
 *  data buffers for later sending to the host as keyboard key presses.
 */
TASK(Magstripe_Read)
{
	/* Arrays to hold the buffer pointers, clock and data bit masks for the seperate card tracks */
	RingBuff_t* TrackBuffer[3]  = {&Track1Data, &Track2Data, &Track3Data};
	uint8_t     TrackClock[3]   = {MAG_T1_CLOCK, MAG_T2_CLOCK, MAG_T3_CLOCK};
	uint8_t     TrackData[3]    = {MAG_T1_DATA,  MAG_T2_DATA,  MAG_T3_DATA};

	/* Previous magnetic card control line' status, for later comparison */
	uint8_t Magstripe_Prev = 0;
	
	/* Buffered current card reader control line' status */
	uint8_t Magstripe_LCL  = Magstripe_GetStatus();

	/* Exit the task early if no card is present in the reader */
	if (!(Magstripe_LCL & MAG_CARDPRESENT))
	  return;

	/* Read out card data while a card is present */
	while (Magstripe_LCL & MAG_CARDPRESENT)
	{
		/* Read out the next bit for each track of the card */
		for (uint8_t Track = 0; Track < 3; Track++)
		{
			/* Current data line status for the current card track */
			bool DataLevel    = ((Magstripe_LCL & TrackData[Track]) != 0);

			/* Current clock line status for the current card track */
			bool ClockLevel   = ((Magstripe_LCL & TrackClock[Track]) != 0);

			/* Current track clock transition check */
			bool ClockChanged = (((Magstripe_LCL ^ Magstripe_Prev) & TrackClock[Track]) != 0);
		
			/* Sample the next bit on the falling edge of the track's clock line, store key code into the track's buffer */
			if (ClockLevel && ClockChanged)
			  Buffer_StoreElement(TrackBuffer[Track], DataLevel ? KEY_1 : KEY_0);
		}

		/* Retain the current card reader control line' status for later edge detection */
		Magstripe_Prev = Magstripe_LCL;
		
		/* Retrieve the new card reader control line states */
		Magstripe_LCL  = Magstripe_GetStatus();
	}
	
	/* Loop through each of the track buffers after the card data has been read */
	for (uint8_t Track = 0; Track < 3; Track++)
	{
		/* Check if the track buffer contains data */
		if (TrackBuffer[Track]->Elements)
		{
			/* Add some enter key presses at the end of each track buffer that contains data */
			Buffer_StoreElement(TrackBuffer[Track], KEY_ENTER);
			Buffer_StoreElement(TrackBuffer[Track], KEY_ENTER);		
		}
	}

	/* Add an extra enter key press to the last track, to seperate out between card reads */
	Buffer_StoreElement(&Track3Data, KEY_ENTER);
}

/** Task for the magnetic card reading and keyboard report generation. This task waits until a card is inserted,
 *  then reads off the card data and sends it to the host as a series of keyboard keypresses via keyboard reports.
 */
TASK(USB_Keyboard_Report)
{
	USB_KeyboardReport_Data_t KeyboardReportData;
	bool                      SendReport = false;
	
	/* Only fetch the next key to send once the period between key presses has elapsed */
	if (!(KeyDelayRemaining))
	{
		/* Create the next keyboard report for transmission to the host */
		SendReport = GetNextReport(&KeyboardReportData);
		
		/* If a key is being sent, reset the key delay period counter */
		if (SendReport)
		  KeyDelayRemaining = 2;
	}
	
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
	}
}
