/*
             LUFA Library
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
	Audio demonstration application. This gives a simple reference
	application for implementing a USB Audio Input device using the
	basic USB Audio drivers in all modern OSes (i.e. no special drivers
	required).
	
	On startup the system will automatically enumerate and function
	as a USB microphone. Incomming audio from the ADC channel 1 will
	be sampled and sent to the host computer.
	
	To use, connect a microphone to the ADC channel 2.
	
	Under Windows, if a driver request dialogue pops up, select the option
	to automatically install the appropriate drivers.

	      ( Input Terminal )--->---( Output Terminal )
	      (   Microphone   )       (  USB Endpoint   )
*/

/*
	USB Mode:           Device
	USB Class:          Audio Class
	USB Subclass:       Standard Audio Device
	Relevant Standards: USBIF Audio Class Specification
	Usable Speeds:      Full Speed Mode
*/

/* ---  Project Configuration  --- */
//#define MICROPHONE_BIASED_TO_HALF_RAIL
/* --- --- --- --- --- --- --- --- */

#include "AudioInput.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,    "LUFA AudioIn App");
BUTTLOADTAG(BuildTime,   __TIME__);
BUTTLOADTAG(BuildDate,   __DATE__);
BUTTLOADTAG(LUFAVersion, "LUFA V" LUFA_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_Audio_Task       , TaskStatus: TASK_STOP },
};

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);
	
	/* Hardware Initialization */
	LEDs_Init();
	ADC_Init(ADC_FREE_RUNNING | ADC_PRESCALE_32);
	ADC_SetupChannel(MIC_IN_ADC_CHANNEL);
	
	/* Start the ADC conversion in free running mode */
	ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | MIC_IN_ADC_CHANNEL);
	
	/* Indicate USB not ready */
	UpdateStatus(Status_USBNotReady);
	
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
	UpdateStatus(Status_USBEnumerating);

	/* Sample reload timer initialization */
	OCR0A   = (F_CPU / AUDIO_SAMPLE_FREQUENCY);
	TCCR0A  = (1 << WGM01);  // CTC mode
	TCCR0B  = (1 << CS00);   // Fcpu speed
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop the sample reload timer */
	TCCR0B = 0;

	/* Stop running audio and USB management tasks */
	Scheduler_SetTaskMode(USB_Audio_Task, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	UpdateStatus(Status_USBNotReady);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup audio stream endpoint */
	Endpoint_ConfigureEndpoint(AUDIO_STREAM_EPNUM, EP_TYPE_ISOCHRONOUS,
		                       ENDPOINT_DIR_IN, AUDIO_STREAM_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Indicate USB connected and ready */
	UpdateStatus(Status_USBReady);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Process General and Audio specific control requests */
	switch (bRequest)
	{
		case REQ_SetInterface:
			/* Set Interface is not handled by the library, as its function is application-specific */
			if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_INTERFACE))
			{
				uint16_t wValue = Endpoint_Read_Word_LE();
				
				Endpoint_ClearSetupReceived();
				
				/* Check if the host is enabling the audio interface (setting AlternateSetting to 1) */
				if (wValue)
				{
					/* Start audio task */
					Scheduler_SetTaskMode(USB_Audio_Task, TASK_RUN);
				}
				else
				{
					/* Stop audio task */
					Scheduler_SetTaskMode(USB_Audio_Task, TASK_STOP);				
				}
				
				/* Handshake the request */
				Endpoint_ClearSetupIN();
			}

			break;
	}
}

/** Function to manage status updates to the user. This is done via LEDs on the given board, if available, but may be changed to
 *  log to a serial port, or anything else that is suitable for status updates.
 *
 *  \param CurrentStatus  Current status of the system, from the StatusCodes_t enum
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
			LEDMask = (LEDS_LED2 | LEDS_LED4);
			break;
	}
	
	/* Set the board LEDs to the new LED mask */
	LEDs_SetAllLEDs(LEDMask);
}

TASK(USB_Audio_Task)
{
	/* Select the audio stream endpoint */
	Endpoint_SelectEndpoint(AUDIO_STREAM_EPNUM);
	
	/* Check if the current endpoint can be read from (contains a packet) */
	if (Endpoint_ReadWriteAllowed())
	{
		/* Process the endpoint bytes all at once; the audio is at such a high sample rate that this
		 * does not have any noticable latency on the USB management task */
		while (Endpoint_BytesInEndpoint() < AUDIO_STREAM_EPSIZE)
		{
			/* Wait until next audio sample should be processed */
			if (!(TIFR0 & (1 << OCF0A)))
				continue;
			else
				TIFR0 |= (1 << OCF0A);

			/* Audio sample is ADC value scaled to fit the entire range */
			int16_t AudioSample = ((SAMPLE_MAX_RANGE / ADC_MAX_RANGE) * ADC_GetResult());
			
#if defined(MICROPHONE_BIASED_TO_HALF_RAIL)
			/* Microphone is biased to half rail voltage, subtract the bias from the sample value */
			AudioSample -= (SAMPLE_MAX_RANGE / 2));
#endif
			
			/* Write the sample to the buffer */
			Endpoint_Write_Word_LE(AudioSample);			
		}
		
		/* Send the full packet to the host */
		Endpoint_ClearCurrentBank();
	}
}
