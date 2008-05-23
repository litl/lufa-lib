/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
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
BUTTLOADTAG(ProjName,     "MyUSB AudioIn App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

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
	
	ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | MIC_IN_ADC_CHANNEL);
	
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
	/* Stop running audio and USB management tasks */
	Scheduler_SetTaskMode(USB_Audio_Task, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup audio stream endpoint */
	Endpoint_ConfigureEndpoint(AUDIO_STREAM_EPNUM, EP_TYPE_ISOCHRONOUS,
		                       ENDPOINT_DIR_IN, AUDIO_STREAM_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);

	/* Start audio task */
	Scheduler_SetTaskMode(USB_Audio_Task, TASK_RUN);
}

EVENT_HANDLER(USB_UnhandledControlPacket)
{
	/* Process General and Audio specific control requests */
	switch (Request)
	{
		case REQ_SetInterface:
			/* Set Interface is not handled by the library, as its function is application-specific */
			if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_INTERFACE))
			{
				Endpoint_ClearSetupReceived();
				Endpoint_Setup_In_Clear();
				while (!(Endpoint_Setup_In_IsReady()));
			}

			break;
	}
}

TASK(USB_Audio_Task)
{
	static bool HasConfiguredTimer = false;
	
	if (USB_IsConnected)
	{
		/* Timers are only set up once after the USB has been connected */
		if (!(HasConfiguredTimer))
		{
			/* Sample reload timer initialization */
			OCR0A   = (F_CPU / AUDIO_SAMPLE_FREQUENCY);
			TCCR0A  = (1 << WGM01);  // CTC mode
			TCCR0B  = (1 << CS00);   // Fcpu speed
			
			HasConfiguredTimer = true;
		}
		
		/* Check to see if the CTC flag is set */
		if (TIFR0 & (1 << OCF0A))
		{
			/* Select the audio stream endpoint */
			Endpoint_SelectEndpoint(AUDIO_STREAM_EPNUM);
			
			/* Check if the current endpoint can be read from (contains a packet) */
			if (Endpoint_ReadWriteAllowed())
			{
				/* Audio sample is ADC value scaled to fit the entire range, then offset to half rail */
				int16_t AudioSample = ((SAMPLE_MAX_RANGE / ADC_MAX_RANGE) * ADC_GetResult());
				
				#if defined(MICROPHONE_BIASED_TO_HALF_RAIL)
				/* Microphone is biased to hald rail voltage, subtract the bias from the sample value */
				AudioSample -= (SAMPLE_MAX_RANGE / 2));
				#endif
				
				/* Write the sample to the buffer */
				Endpoint_Write_Word_LE(AudioSample);

				/* Check to see if all bytes in the current endpoint have been written, if so send the data */
				if (Endpoint_BytesInEndpoint() == AUDIO_STREAM_EPSIZE)
				  Endpoint_FIFOCON_Clear();
			}

			/* Clear the CTC flag */
			TIFR0 |= (1 << OCF0A);
		}
	}
	else
	{
		/* Stop the timers */
		TCCR0B = 0;
		
		HasConfiguredTimer = false;
	}
}
