/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Audio demonstration application. This gives a simple reference
	application for implementing a USB Audio Output device using the
	basic USB Audio drivers in all modern OSes (i.e. no special drivers
	required).
	
	On startup the system will automatically enumerate and function
	as a USB speaker. Incomming audio will output in 8-bit PWM onto
	Timer 3 output compare channel A for AUDIO_OUT_MONO mode, on
	channels A and B for AUDIO_OUT_STEREO and on the two bicolour LEDs
	for AUDIO_OUT_LEDS mode. Decouple audio outputs with a capacitor
	and attach to a speaker to hear the audio.
	
	Under Windows, if a driver request dialogue pops up, select the option
	to automatically install the appropriate drivers.
	
	      ( Input Terminal )--->---( Output Terminal )
	      (  USB Endpoint  )       (     Speaker     )
*/

/* ---  Project Configuration  --- */
//#define AUDIO_OUT_MONO
//#define AUDIO_OUT_STEREO
#define AUDIO_OUT_LEDS
/* --- --- --- --- --- --- --- --- */

#include "AudioOutput.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB AudioOut App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ Task: USB_Audio_Task       , TaskStatus: TASK_RUN  },
};

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Bicolour_Init();
	
	/* Initial LED colour - Double red to indicate USB not ready */
	Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
	
	/* Initialize USB Subsystem */
	USB_Init(USB_MODE_DEVICE, USB_DEV_OPT_HIGHSPEED | USB_OPT_REG_ENABLED);

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup audio stream endpoint */
	Endpoint_ConfigureEndpoint(AUDIO_STREAM_EPNUM, EP_TYPE_ISOCHRONOUS,
		                       ENDPOINT_DIR_OUT, AUDIO_STREAM_EPSIZE,
	                           ENDPOINT_BANK_DOUBLE);

	/* Double green to indicate USB connected and ready */
	Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN);
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
	static bool HasConfiguredTimers = false;
	
	if (USB_IsConnected)
	{
		/* Timers are only set up once after the USB has been connected */
		if (!(HasConfiguredTimers))
		{
			/* Sample reload timer initialization */
			OCR0A   = (F_CPU / AUDIO_SAMPLE_FREQUENCY);
			TCCR0A  = (1 << WGM01);  // CTC mode
			TCCR0B  = (1 << CS00);   // Fcpu speed
			
#if (defined(AUDIO_OUT_MONO) || defined(AUDIO_OUT_STEREO))
			/* PWM speaker timer initialization */
			TCCR3A  = ((1 << WGM30) | (1 << COM3A1) | (1 << COM3A0)
			                        | (1 << COM3B1) | (1 << COM3B0)); // Set on match, clear on TOP
			TCCR3B  = ((1 << CS30));  // Phase Correct 8-Bit PWM, Fcpu speed
#endif

#if defined(AUDIO_OUT_MONO)
			/* Set speaker as output */
			DDRC   |= (1 << 6);
#elif defined(AUDIO_OUT_STEREO)
			/* Set speakers as outputs */
			DDRC   |= ((1 << 6) | (1 << 5));
#endif

			HasConfiguredTimers = true;
		}
		
		/* Check to see if the CTC flag is set */
		if (TIFR0 & (1 << OCF0A))
		{
			/* Select the audio stream endpoint */
			Endpoint_SelectEndpoint(AUDIO_STREAM_EPNUM);
			
			/* Check if the current endpoint can be read from (contains a packet) */
			if (Endpoint_ReadWriteAllowed())
			{
				/* Retrieve the signed 16-bit left and right audio samples */
				int16_t LeftSample_16Bit  = (int16_t)Endpoint_Read_Word_LE();
				int16_t RightSample_16Bit = (int16_t)Endpoint_Read_Word_LE();

				/* Massage signed 16-bit left and right audio samples into signed 8-bit */
				int8_t  LeftSample_8Bit   = (LeftSample_16Bit  >> 8);
				int8_t  RightSample_8Bit  = (RightSample_16Bit >> 8);
				
#if defined(AUDIO_OUT_MONO)
				/* Mix the two channels together to produce a mono, 8-bit sample */
				int8_t  MixedSample_8Bit  = (((int16_t)LeftSample_8Bit + (int16_t)RightSample_8Bit) >> 1);

				/* Load the sample into the PWM timer channel */
				OCR3A = (MixedSample_8Bit ^ (1 << 7));
#elif defined(AUDIO_OUT_STEREO)
				/* Load the dual 8-bit samples into the PWM timer channels */
				OCR3A = (LeftSample_8Bit  ^ (1 << 7));
				OCR3B = (RightSample_8Bit ^ (1 << 7));
#else
				/* Make left channel positive (absolute) */
				if (LeftSample_8Bit < 0)
				  LeftSample_8Bit = -LeftSample_8Bit;

				/* Make right channel positive (absolute) */
				if (RightSample_8Bit < 0)
				  RightSample_8Bit = -RightSample_8Bit;

				/* Set first LED based on sample value */
				if (LeftSample_8Bit == 0)
				  Bicolour_SetLed(1, BICOLOUR_LED1_OFF);
				else if (LeftSample_8Bit < ((128 / 8) * 1))
				  Bicolour_SetLed(1, BICOLOUR_LED1_GREEN);
				else if (LeftSample_8Bit < ((128 / 8) * 3))
				  Bicolour_SetLed(1, BICOLOUR_LED1_ORANGE);
				else
				  Bicolour_SetLed(1, BICOLOUR_LED1_RED);

				/* Set second LED based on sample value */
				if (RightSample_8Bit == 0)
				  Bicolour_SetLed(2, BICOLOUR_LED2_OFF);
				else if (RightSample_8Bit < ((128 / 8) * 1))
				  Bicolour_SetLed(2, BICOLOUR_LED2_GREEN);
				else if (RightSample_8Bit < ((128 / 8) * 3))
				  Bicolour_SetLed(2, BICOLOUR_LED2_ORANGE);
				else
				  Bicolour_SetLed(2, BICOLOUR_LED2_RED);
#endif

				/* Check to see if all bytes in the current endpoint have been read, if so clear the endpoint */
				if (!(Endpoint_BytesInEndpoint()))
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
#if (defined(AUDIO_OUT_MONO) || defined(AUDIO_OUT_STEREO))
		TCCR3B = 0;
#endif		
		HasConfiguredTimers = false;
	}
}
