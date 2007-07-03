/*

    MyUSB Test Application

*/

#include "TestApp.h"

/* Scheduler Task List: */
	TASK_ID_LIST
	{
		TestApp_CheckJoystick_ID,
		USB_ManagementTask_ID,
	};

	TASK_LIST
	{
		{ TaskID: TestApp_CheckJoystick_ID, TaskName: TestApp_CheckJoystick, TaskStatus: TASK_RUN },
		{ TaskID: USB_ManagementTask_ID   , TaskName: USB_ManagementTask   , TaskStatus: TASK_RUN }
	};


int main(void)
{
	/* Hardware Initialization */
	Joystick_Init();
	HWB_Init();
	Bicolour_Init();
	USB_Init(USB_MODE_DEVICE);
	
	/* Millisecond Timer Initialization */
	OCR0A  = 0x7D;
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS01) | (1 << CS00));
	
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
	SchedulerDelayCounter_t MouseDelay;
	
	if (Scheduler_HasDelayElapsed(10, MouseDelay))
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

		/* Test of the hardware button - invert LEDs when pressed */
		if (HWB_GetStatus() == true)
		  LEDMask ^= BICOLOUR_ALL_LEDS;

		/* Test of bi-colour LED - light up in response to joystick */
		Bicolour_SetLeds(LEDMask);
	}
}
