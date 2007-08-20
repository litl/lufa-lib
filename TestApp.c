/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "TestApp.h"

int main(void)
{
	/* Disable clock division */
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;

	/* Hardware Initialization */
	Joystick_Init();
	HWB_Init();
	Bicolour_Init();
	Serial_Init(9600);
	
	/* Millisecond Timer Initialization */
	OCR0A  = 0x7D;
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS01) | (1 << CS00));
	TIMSK0 = (1 << OCIE0A);
	
	/* Turn on interrupts */
	sei();

    /* Startup message via USART */
	printf("MyUSB Demo running.\r\n\n");
		
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
					USB_ShutDown();

					printf("USB Power Off.\r\n");				
					
					Bicolour_SetLeds(BICOLOUR_LED1_RED);
				}
				else
				{
					Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_RED);

					printf("USB Power On.\r\n");
				
					USB_Init(USB_MODE_UID, USB_DEV_HIGHSPEED);
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
