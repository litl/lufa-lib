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
	to automatically install the appropriate drivers, or manually select the
	USBAudio.sys driver.
*/

/*
	THIS EXAMPLE IS UNFINISHED AND NON OPERATIONAL. FOR DEVELOPMENT PURPOSES ONLY.
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

int main(void)
{
	/* Disable Clock Division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Joystick_Init();
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

TASK(USB_Audio_Task)
{
	static bool HasConfiguredTimer = false;
	
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

ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	uint8_t PrevEPNum = Endpoint_GetCurrentEndpoint();

	Endpoint_SelectEndpoint(AUDIO_STREAM_EPNUM);
	if (Endpoint_ReadWriteAllowed())
	{
		uint16_t Sample = Endpoint_Read_Word_LE();
		
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
	}
	
	Endpoint_SelectEndpoint(PrevEPNum);
}

