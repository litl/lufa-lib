/*

           MyUSB Test Application
	        By Dean Camera, 2007
	dean (at) fourwalledcubicle (dot) com	
*/

#include "TestApp.h"

/* Scheduler Task List: */
	TASK_ID_LIST
	{
		TestApp_CheckJoystick_ID,
		TestApp_CheckHWB_ID,
		USB_USBTask_ID,
	};

	TASK_LIST
	{
		{ TaskID: TestApp_CheckJoystick_ID, TaskName: TestApp_CheckJoystick, TaskStatus: TASK_RUN },
		{ TaskID: TestApp_CheckHWB_ID     , TaskName: TestApp_CheckHWB     , TaskStatus: TASK_RUN },
		{ TaskID: USB_USBTask_ID          , TaskName: USB_USBTask          , TaskStatus: TASK_RUN },
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
	Scheduler_TickCounter++;
}


TASK(TestApp_CheckJoystick)
{
	uint8_t JoyStatus_LCL = Joystick_GetStatus();
	uint8_t LEDMask       = 0;
		
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

TASK(TestApp_CheckHWB)
{
	static SchedulerDelayCounter_t LastTick;
	static uint8_t                 ConsecutiveTicks;
	static bool                    IsPressed;
	static bool                    BlockingJoystickTask;
	
	if (HWB_GetStatus() == true)
	{
		if (IsPressed == false)
		{
			if (Scheduler_TickCounter != LastTick)
			{
				ConsecutiveTicks++;
				LastTick = Scheduler_TickCounter;
			}

			if (ConsecutiveTicks == 100)
			{
				IsPressed = true;
				
				if (BlockingJoystickTask == false)
				{
					Scheduler_SetTaskMode(TestApp_CheckJoystick_ID, TASK_STOP);
					BlockingJoystickTask = true;
				}

				if (USB_IsInitialized == true)
				{
					USB_PowerOff();
					Bicolour_SetLeds(BICOLOUR_LED1_RED);
				}
				else
				{
					USB_Init(USB_MODE_DEVICE, USB_DEV_HIGHSPEED);
					Bicolour_SetLeds(BICOLOUR_LED1_GREEN);
				}
			}
		}
	}
	else
	{
		ConsecutiveTicks = 0;
		IsPressed        = false;
	}
}
