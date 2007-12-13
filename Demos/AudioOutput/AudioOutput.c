/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Audio demonstration application. This gives a simple reference
	application for implementing a USB Audio Output device using the
	basic USB Audio drivers in all modern OSes (i.e. no special drivers
	required).
	
	On startup the system will automatically enumerate and function
	as a USB speaker. Incomming audio will output in 8-bit PWM onto
	Timer 3 output compare channel A. Decouple the output with a capacitor
	and attach to a speaker to hear the audio.
	
	Under Windows, if a driver request dialogue pops up, select the option
	to automatically install the appropriate drivers.
*/

#include "AudioOutput.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB AudioOut App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task ID list */
TASK_ID_LIST
{
	USB_USBTask_ID,
	USB_Audio_Task_ID,
};

/* Scheduler Task List */
TASK_LIST
{
	{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN  },
	{ TaskID: USB_Audio_Task_ID       , TaskName: USB_Audio_Task       , TaskStatus: TASK_RUN  },
};

/* Globals */
volatile bool    Mute;
volatile int16_t Volume;

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Joystick_Init();
	Bicolour_Init();
	SerialStream_Init(9600);
	
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
	/* Process Audio specific control requests */
	switch (Request)
	{
		case GET_MAX:
		case GET_MIN:
		case GET_RES:
		case GET_CUR:
			if ((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_TYPE))
			     == (REQDIR_DEVICETOHOST | REQTYPE_CLASS))
			{
				uint8_t FeatureToGet;
				
				Endpoint_Ignore_Byte();
				FeatureToGet = Endpoint_Read_Byte();
				Endpoint_Ignore_DWord();

				Endpoint_ClearSetupReceived();

				switch (FeatureToGet)
				{
					case GET_SET_MUTE:
						Endpoint_Write_Byte(Mute);

						break;
					case GET_SET_VOLUME:
						if (Request == GET_MAX)
							Endpoint_Write_Word_LE(VOL_MAX);
						else if (Request == GET_MIN)
							Endpoint_Write_Word_LE(VOL_MIN);
						else if (Request == GET_RES)
							Endpoint_Write_Word_LE(VOL_RES);
						else
							Endpoint_Write_Word_LE(Volume);

						break;
				}
				
				Endpoint_In_Clear();
			
				while (!(Endpoint_Out_IsReceived()));
				Endpoint_Out_Clear();
			}
			
			break;
		case SET_CUR:
			if ((RequestType & (CONTROL_REQTYPE_DIRECTION | CONTROL_REQTYPE_TYPE))
			     == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS))
			{
				uint8_t FeatureToSet;
				
				Endpoint_Ignore_Byte();
				FeatureToSet = Endpoint_Read_Byte();
				Endpoint_Ignore_DWord();

				Endpoint_ClearSetupReceived();	

				Endpoint_In_Clear();				
				while (!(Endpoint_Out_IsReceived()));
				
				switch (FeatureToSet)
				{
					case GET_SET_MUTE:
						Mute = Endpoint_Read_Byte();
						break;
					case GET_SET_VOLUME:
						Volume = Endpoint_Read_Word_LE();
						break;
				}
				
				Endpoint_Out_Clear();
				
				while (!(Endpoint_In_IsReady()));
				Endpoint_In_Clear();
			}
			
			break;
	}
}

TASK(USB_Audio_Task)
{
	static bool HasConfiguredTimer = false;
	
	if (USB_IsConnected)
	{
		if (!(HasConfiguredTimer))
		{
			/* 20uS sample timer initialization */
			OCR0A  = 20;
			TCCR0A = (1 << WGM01);  // CTC mode
			TCCR0B = (1 << CS01);
			TIMSK0 = (1 << OCIE0A); // CTC interrupt enable
			
			HasConfiguredTimer = true;
			
			/* PWM speaker timer initialization */
			TCCR3A  = ((1 << WGM30) | (1 << COM3A1) | (1 << COM3A0)); // Set on match, clear on TOP
			TCCR3B  = ((1 << CS30));  // Phase Correct 8-Bit PWM, Fcpu speed

			/* Set speaker as output */
			DDRC   |= (1 << 6);
		}
	}
	else
	{
		/* Stop the timer */
		TCCR0B = 0;
		HasConfiguredTimer = false;
	}
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	/* Save the currently selected endpoint */
	uint8_t PrevEPNum = Endpoint_GetCurrentEndpoint();

	/* Select the audio stream endpoint */
	Endpoint_SelectEndpoint(AUDIO_STREAM_EPNUM);
	
	/* Check if the current endpoint can be read from (contains a packet) */
	if (Endpoint_ReadWriteAllowed())
	{
		int16_t LeftSample  = (int16_t)Endpoint_Read_Word_LE();
		int16_t RightSample = (int16_t)Endpoint_Read_Word_LE();
		int8_t  MixedSample = ((LeftSample >> 9) + (RightSample >> 9)) ^ (1 << 7);
      
		/* Load the sample into the PWM timer */
		OCR3A = MixedSample;

		/* Check to see if all bytes in the current endpoint have been read, if so clear the endpoint */
		if (!(Endpoint_BytesInEndpoint()))
		  Endpoint_In_Clear();
	}
	
	/* Select the previously selected endpoint again */
	Endpoint_SelectEndpoint(PrevEPNum);
}

