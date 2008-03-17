/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Test application. Demonstrates several aspects of the MyUSB
	Library. On startup the current temperature will be printed
	through the USART every 10 seconds, and the current joystick
	position will be indicated via the two Bicolour LEDs on the
	USBKEY board. Pressing the HWB on the USBKEY will initiate
	the USB subsystem, enumerating the device (which has no
	actual functionality beyond enumeration as a device or as a
	host in this demo, and serves only to demonstrate the USB portion
	of the library). It will also suspend the joystick and
	temperature monitoring tasks.
	
	Pressing the HWB a second time will turn off the USB system
	and resume the temperature printing task (but not the joystick
	monitoring task).
	
	When activated, the USB events will be printed through the
	serial USART.
	
	When the USB subsystem is activated, the USBKEY's status LEDs
	will show the current USB status, as described in the following
	table:
	
	  COLOUR:          | DESCRIPTION:
	 ------------------+-------------------------------------------
	  Off/Off          | Joystick monitoring task on, USB off.
	  Red/Red          | USB error.
	  Red/Off          | USB not activated.
	  Green/Red        | USB activated, but not connected.
	  Green/Orange     | USB connected, but not initialized.
	  Green/Green      | USB initialized.
	  Orange/Orange    | USB suspended.
*/

#include "TestApp.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,  "MyUSB Test App");
BUTTLOADTAG(BuildTime, __TIME__);
BUTTLOADTAG(BuildDate, __DATE__);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: TestApp_CheckJoystick, TaskStatus: TASK_RUN  },
	{ Task: TestApp_CheckHWB     , TaskStatus: TASK_RUN  },
	{ Task: TestApp_CheckTemp    , TaskStatus: TASK_RUN  },
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
};

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	SetSystemClockPrescaler(0);
	
	/* Hardware initialization */
	SerialStream_Init(9600);
	ADC_Init(ADC_SINGLE_CONVERSION | ADC_PRESCALE_64);
	Temperature_Init();
	Joystick_Init();
	Bicolour_Init();
	HWB_Init();
	
	/* Millisecond timer initialization, with output compare interrupt enabled */
	OCR0A  = 0x7D;
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS01) | (1 << CS00));
	TIMSK0 = (1 << OCIE0A);
	
	/* Turn on interrupts */
	sei();

    /* Startup message via USART */
	puts_P(PSTR(ESC_RESET ESC_BG_WHITE ESC_INVERSE_ON ESC_ERASE_DISPLAY
	       "MyUSB Demo running.\r\n" ESC_INVERSE_OFF));

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}


ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	/* Scheduler test - increment scheduler tick counter once each millisecond */
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

TASK(TestApp_CheckTemp)
{
	static SchedulerDelayCounter_t DelayCounter = 10000; // Force immediate run on startup

	/* Task runs every 10000 ticks, 10 seconds for this demo */
	if (Scheduler_HasDelayElapsed(10000, &DelayCounter))
	{
		printf_P(PSTR("Current temperature: %d Degrees Celcius\r\n\r\n"),
		         (int)Temperature_GetTemperature());

		/* Reset the delay counter, ready to count another 10000 tick interval */
		Scheduler_ResetDelay(&DelayCounter);
	}	
}

TASK(TestApp_CheckHWB)
{
	static SchedulerDelayCounter_t DelayCounter = 0;
	static bool                    IsPressed;
	static bool                    BlockingJoystickTask;
	
	/* Check if HWB pressed (start USB) */
	if (HWB_GetStatus() == true)
	{
		/* Debounce - check 100 ticks later to see if button is still being pressed */
		if ((IsPressed == false) && (Scheduler_HasDelayElapsed(100, &DelayCounter)))
		{
			/* Set flag, indicating that current pressed state has been handled */
			IsPressed = true;
			
			/* First start of the USB interface permenantly blocks the joystick task */
			if (BlockingJoystickTask == false)
			{
				Scheduler_SetTaskMode(TestApp_CheckJoystick, TASK_STOP);
				BlockingJoystickTask = true;
			}

			/* Toggle USB interface */
			if (USB_IsInitialized == true)
			{
				USB_ShutDown();

				Bicolour_SetLeds(BICOLOUR_LED1_RED);
				puts_P(PSTR(ESC_BG_WHITE "USB Power Off.\r\n"));
				
				Scheduler_SetTaskMode(TestApp_CheckTemp, TASK_RUN);
			}
			else
			{
				Scheduler_SetTaskMode(TestApp_CheckTemp, TASK_STOP);

				Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_RED);
				puts_P(PSTR(ESC_BG_YELLOW "USB Power On.\r\n"));
				
				USB_Init(USB_MODE_UID, USB_DEV_OPT_FULLSPEED | USB_OPT_REG_ENABLED);
			}
		}
	}
    else
    {
		/* HWB not pressed - reset debounce interval counter and press handled flag */
		Scheduler_ResetDelay(&DelayCounter);
		IsPressed = false;
	}
}
