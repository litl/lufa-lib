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
	application for implementing a USB Audio Output device using the
	basic USB Audio drivers in all modern OSes (i.e. no special drivers
	required).
	
	On startup the system will automatically enumerate and function
	as a USB speaker. Incomming audio will output in 8-bit PWM onto
	the timer output (timer 3 for the AT90USBXXX6/7 USB AVRs, timer 1 for
	the AT90USBXXX2 controller AVRs) compare channel A for AUDIO_OUT_MONO
	mode, on channels A and B for AUDIO_OUT_STEREO and on the board LEDs
	for AUDIO_OUT_LEDS mode. Decouple audio outputs with a capacitor and
	attach to a speaker to hear the audio.
	
	Under Windows, if a driver request dialogue pops up, select the option
	to automatically install the appropriate drivers.
	
	      ( Input Terminal )--->---( Output Terminal )
	      (  USB Endpoint  )       (     Speaker     )
*/

/*
	USB Mode:           Device
	USB Class:          Audio Class
	USB Subclass:       Standard Audio Device
	Relevant Standards: USBIF Audio Class Specification
	Usable Speeds:      Full Speed Mode
*/

/* ---   Project Configuration (Choose ONE)   --- */
//#define AUDIO_OUT_MONO
//#define AUDIO_OUT_STEREO
#define AUDIO_OUT_LEDS
//#define AUDIO_OUT_PORTC
/* --- --- --- --- --- --- --- ---  ---  ---  --- */

#include "AudioOutput.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,    "LUFA AudioOut App");
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
			
#if defined(AUDIO_OUT_MONO)
	/* Set speaker as output */
	DDRC   |= (1 << 6);
#elif defined(AUDIO_OUT_STEREO)
	/* Set speakers as outputs */
	DDRC   |= ((1 << 6) | (1 << 5));
#elif defined(AUDIO_OUT_PORTC)
	/* Set PORTC as outputs */
	DDRC   |= 0xFF;
#endif

#if (defined(AUDIO_OUT_MONO) || defined(AUDIO_OUT_STEREO))
	/* PWM speaker timer initialization */
	TCCRxA  = ((1 << WGMx0) | (1 << COMxA1) | (1 << COMxA0)
							| (1 << COMxB1) | (1 << COMxB0)); // Set on match, clear on TOP
	TCCRxB  = ((1 << CSx0));  // Phase Correct 8-Bit PWM, Fcpu speed
#endif	
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop the timers */
	TCCR0B = 0;
#if (defined(AUDIO_OUT_MONO) || defined(AUDIO_OUT_STEREO))
	TCCRxB = 0;
#endif		

#if defined(AUDIO_OUT_MONO)
	/* Set speaker as input to reduce current draw */
	DDRC   &= ~(1 << 6);
#elif defined(AUDIO_OUT_STEREO)
	/* Set speakers as inputs to reduce current draw */
	DDRC   &= ~((1 << 6) | (1 << 5));
#elif defined(AUDIO_OUT_PORTC)
	/* Set PORTC low */
	PORTC  = 0x00;
#endif

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
		                       ENDPOINT_DIR_OUT, AUDIO_STREAM_EPSIZE,
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
 *  \param CurrentStatus  Current status of the system, from the AudioOutput_StatusCodes_t enum
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
		while (Endpoint_BytesInEndpoint())
		{
			/* Wait until next audio sample should be processed */
			if (!(TIFR0 & (1 << OCF0A)))
				continue;
			else
				TIFR0 |= (1 << OCF0A);

			/* Retreive the signed 16-bit left and right audio samples */
			int16_t LeftSample_16Bit  = (int16_t)Endpoint_Read_Word_LE();
			int16_t RightSample_16Bit = (int16_t)Endpoint_Read_Word_LE();

			/* Massage signed 16-bit left and right audio samples into signed 8-bit */
			int8_t  LeftSample_8Bit   = (LeftSample_16Bit  >> 8);
			int8_t  RightSample_8Bit  = (RightSample_16Bit >> 8);
			
#if defined(AUDIO_OUT_MONO)
			/* Mix the two channels together to produce a mono, 8-bit sample */
			int8_t  MixedSample_8Bit  = (((int16_t)LeftSample_8Bit + (int16_t)RightSample_8Bit) >> 1);

			/* Load the sample into the PWM timer channel */
			OCRxA = ((uint8_t)MixedSample_8Bit ^ (1 << 7));
#elif defined(AUDIO_OUT_STEREO)
			/* Load the dual 8-bit samples into the PWM timer channels */
			OCRxA = ((uint8_t)LeftSample_8Bit  ^ (1 << 7));
			OCRxB = ((uint8_t)RightSample_8Bit ^ (1 << 7));
#elif defined(AUDIO_OUT_PORTC)
			/* Mix the two channels together to produce a mono, 8-bit sample */
			int8_t  MixedSample_8Bit  = (((int16_t)LeftSample_8Bit + (int16_t)RightSample_8Bit) >> 1);

			PORTC = MixedSample_8Bit;
#else
			uint8_t LEDMask = LEDS_NO_LEDS;

			/* Make left channel positive (absolute) */
			if (LeftSample_8Bit < 0)
			  LeftSample_8Bit = -LeftSample_8Bit;

			/* Make right channel positive (absolute) */
			if (RightSample_8Bit < 0)
			  RightSample_8Bit = -RightSample_8Bit;

			/* Set first LED based on sample value */
			if (LeftSample_8Bit < ((128 / 8) * 1))
			  LEDMask |= LEDS_LED2;
			else if (LeftSample_8Bit < ((128 / 8) * 3))
			  LEDMask |= (LEDS_LED1 | LEDS_LED2);
			else
			  LEDMask |= LEDS_LED1;

			/* Set second LED based on sample value */
			if (RightSample_8Bit < ((128 / 8) * 1))
			  LEDMask |= LEDS_LED4;
			else if (RightSample_8Bit < ((128 / 8) * 3))
			  LEDMask |= (LEDS_LED3 | LEDS_LED4);
			else
			  LEDMask |= LEDS_LED3;
			  
			LEDs_SetAllLEDs(LEDMask);
#endif
		}
		
		/* Acknowedge the packet, clear the bank ready for the next packet */
		Endpoint_ClearCurrentBank();
	}
}
