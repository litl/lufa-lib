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
	as a USB speaker. Incomming audio will be displayed on the two bicolour
	LEDs on the USBKEY board.
	
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
	printf("%d - ", Request);

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
						
						printf("Get curr mute \r\n");

						break;
					case GET_SET_VOLUME:
						if (Request == GET_MAX)
						{
							Endpoint_Write_Word_LE(VOL_MAX);
							printf("Get vol max \r\n");
						}
						else if (Request == GET_MIN)
						{
							Endpoint_Write_Word_LE(VOL_MIN);
							printf("Get vol min \r\n");
						}
						else if (Request == GET_RES)
						{
							Endpoint_Write_Word_LE(VOL_RES);
							printf("Get vol res \r\n");
						}
						else
						{
							Endpoint_Write_Word_LE(Volume);
							printf("Get curr vol \r\n");
						}

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
						printf("Set mute val \r\n");
						break;
					case GET_SET_VOLUME:
						Volume = Endpoint_Read_Word_LE();
						printf("Set vol val \r\n");
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
			/* 125uS timer initialization */
			OCR0A  = 125;
			TCCR0A = (1 << WGM01);
			TCCR0B = (1 << CS01);
			TIMSK0 = (1 << OCIE0A);
			
			HasConfiguredTimer = true;
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
		/* Get the left and right channel audio samples from the endpoint */
		uint16_t LeftSample  = Endpoint_Read_Word_LE();
		uint16_t RightSample = Endpoint_Read_Word_LE();
		
		/* Create a mono sample from the two channels */
		uint16_t Sample = ((LeftSample >> 1) + (RightSample >> 1));
		
		/* If mute is enabled, clear the sample value */
		if (Mute || (Volume == VOL_SILENCE))
		  Sample = 0;
		  
		/* Display the sample value on the bicolour LEDs */
		if (Sample <= (0xFFFF / 7))
		  Bicolour_SetLeds(BICOLOUR_NO_LEDS);
		else if (Sample <= ((0xFFFF / 7) * 2))
		  Bicolour_SetLeds(BICOLOUR_LED1_GREEN);
		else if (Sample <= ((0xFFFF / 7) * 3))
		  Bicolour_SetLeds(BICOLOUR_LED1_ORANGE);
		else if (Sample <= ((0xFFFF / 7) * 4))
		  Bicolour_SetLeds(BICOLOUR_LED1_RED);
		else if (Sample <= ((0xFFFF / 7) * 5))
		  Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_GREEN);
		else if (Sample <= ((0xFFFF / 7) * 6))
		  Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_ORANGE);
		else
		  Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_RED);
		  
		/* Check to see if all bytes in the current endpoint have been read, if so clear the endpoint */
		if (!(Endpoint_BytesInEndpoint()))
		  Endpoint_In_Clear();
	}
	
	/* Select the previously selected endpoint again */
	Endpoint_SelectEndpoint(PrevEPNum);
}

