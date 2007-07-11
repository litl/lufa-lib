/*

           MyUSB Test Application
	        By Dean Camera, 2007
	dean (at) fourwalledcubicle (dot) com
	


	This is a demonstration of the MyUSB library. It shows off many features of the MyUSB
	framework.
	
	On startup, two tasks are running - the Joystick test routine and the HWB test routine.
	Upon moving the joystick, the current joystick position is displayed on the two bicolour
	LEDs of the USBKEY.
	
	When the HWB is pressed, the HWB and joystick tasks are stopped, and the USB connection
	checking routine executes. When VBUS is detected (device plugged into a host), the LEDs
	will alternate between red and green.
*/

#include "TestApp.h"

/* Scheduler Task List: */
	TASK_ID_LIST
	{
		TestApp_CheckJoystick_ID,
		TestApp_HWBTest_ID,
		TestApp_USBConnCheck_ID,
	};

	TASK_LIST
	{
		{ TaskID: TestApp_CheckJoystick_ID, TaskName: TestApp_CheckJoystick, TaskStatus: TASK_RUN  },
		{ TaskID: TestApp_HWBTest_ID      , TaskName: TestApp_HWBTest      , TaskStatus: TASK_RUN  },
		{ TaskID: TestApp_USBConnCheck_ID , TaskName: TestApp_USBConnCheck , TaskStatus: TASK_STOP }
	};


int main(void)
{
	/* Disable clock division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Joystick_Init();
	HWB_Init();
	Bicolour_Init();
	USB_Init(USB_MODE_DEVICE);
	
	/* Millisecond Timer Initialization */
	OCR0A  = 0x7D;
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS01) | (1 << CS00));
	TIMSK0 = (1 << OCIE0A);
	
	/* Turn on interrupts */
	sei();
		
	/* Scheduling */
	Scheduler_Start(); // Scheduler never returns, so put this last
}


ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	/* Scheduler test - increment scheduler delay counter once each millisecond */
	Scheduler_IncrementElapsedDelay();
}

TASK(TestApp_CheckJoystick)
{
	uint8_t JoyStatus_LCL = Joystick_GetStatus();
	uint8_t LEDMask = 0;
		
	/* Test of the Joystick - change a mask in response to joystick */
	if (JoyStatus_LCL & JOY_UP)
	  LEDMask |= BICOLOUR_LED1_RED;
		
	if (JoyStatus_LCL & JOY_DOWN)
	  LEDMask |= BICOLOUR_LED1_GREEN;

	if (JoyStatus_LCL & JOY_LEFT)
	  LEDMask |= BICOLOUR_LED2_RED;

	if (JoyStatus_LCL & JOY_RIGHT)
	  LEDMask |= BICOLOUR_LED2_GREEN;
			
	if (JoyStatus_LCL & JOY_PRESS)
	  LEDMask  = BICOLOUR_ALL_LEDS;

	/* Test of bi-colour LED - light up in response to joystick */
	Bicolour_SetLeds(LEDMask);
}

TASK(TestApp_HWBTest)
{
	/* Test of the HWB - start USB when pressed */
	if (HWB_GetStatus() == true)
	{
		Scheduler_SetTaskMode(TestApp_USBConnCheck_ID, TASK_RUN);
		Scheduler_SetTaskMode(TestApp_CheckJoystick_ID, TASK_STOP);
		Scheduler_SetTaskMode(TestApp_HWBTest_ID, TASK_STOP);

		/* Start USB */
		if (USB_PowerOn() != USB_POWERON_OK)
		{
			Bicolour_SetLeds(BICOLOUR_LED1_RED);
			for (;;);
		}
		else
		{
			Bicolour_SetLeds(BICOLOUR_LED1_GREEN);		
		}
	}
}

TASK(TestApp_USBConnCheck)
{
	SchedulerDelayCounter_t LEDDelay;
	static bool LedToggle;
		  
	if (USB_VBUS_GetStatus() == true)
	{
		if (Scheduler_HasDelayElapsed(200, &LEDDelay) == true)
		{
			if (LedToggle)
			  Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_RED);
			else
			  Bicolour_SetLeds(BICOLOUR_LED1_RED | BICOLOUR_LED2_GREEN);
			  
			LedToggle = !(LedToggle);
		}
	}
}
