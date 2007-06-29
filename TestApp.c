/*

    MyUSB Test Application

*/

#include "TestApp.h"

/* Scheduler Task List: */
	TASK_LIST
	{
		TestApp_CheckJoystick,
		TestApp_Delay,
		USB_ManagementTask // Required to manage USB
	};



int main(void)
{
	/* Initialization */
	Joystick_Init();
	HWB_Init();
	Bicolour_Init();
	USB_Init();
		
	/* Scheduling */
	Scheduler_Start(); // Scheduler never returns, so put this last
}

TASK(TestApp_CheckJoystick)
{
	static SchedulerDelayCounter_t MouseDelay;
	
	if (Scheduler_HasDelayElapsed(10, MouseDelay))
	{
		uint8_t JoyStatus_LCL = Joystick_GetStatus();
		uint8_t LEDMask = 0;
		
		
		/* Test of the hardware button - invert joystick status when pressed */
		if (HWB_GetStatus() == true)
		  JoyStatus_LCL ^= (JOY_UP | JOY_DOWN | JOY_LEFT | JOY_RIGHT | JOY_PRESS);

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
}

#include <util/delay.h>

TASK(TestApp_Delay)
{
	/* Scheduler test - increment scheduler once a millisecond via a fixed delay */
	_delay_ms(1);
	Scheduler_IncrementElapsedDelay();
}
