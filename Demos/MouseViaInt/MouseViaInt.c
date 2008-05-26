/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	Mouse demonstration application, using endpoint interrupts. This
	gives a simple reference application for implementing a USB Mouse
	using the basic USB HID drivers in all modern OSes (i.e. no special
	drivers required).
	
	On startup the system will automatically enumerate and function
	as a mouse when the USB connection to a host is present. To use
	the mouse, move the joystick to move the pointer, and push the
	joystick inwards to simulate a left-button click. The HWB serves as
	the right mouse button.
*/

/*
	USB Mode:           Device
	USB Class:          Human Interface Device (HID)
	USB Subclass:       Mouse
	Relevant Standards: USBIF HID Standard
	                    USBIF HID Usage Tables 
	Usable Speeds:      Low Speed Mode, Full Speed Mode
*/

#include "MouseViaInt.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB MouseI App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
};

int main(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable Clock Division */
	SetSystemClockPrescaler(0);

	/* Hardware Initialization */
	Joystick_Init();
	LEDs_Init();
	HWB_Init();
	
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
	
	/* Initialize Scheduler so that it can be used */
	Scheduler_Init();

	/* Initialize USB Subsystem */
	USB_Init();

	/* Scheduling - routine never returns, so put this last in the main function */
	Scheduler_Start();
}

EVENT_HANDLER(USB_Connect)
{
	/* Start USB management task */
	Scheduler_SetTaskMode(USB_USBTask, TASK_RUN);

	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED4);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running mouse reporting and USB management tasks */
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup Mouse Report Endpoint */
	Endpoint_ConfigureEndpoint(MOUSE_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, MOUSE_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Enable the endpoint IN interrupt ISR for the report endpoint */
	USB_INT_Enable(ENDPOINT_INT_IN);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);
}

ISR(ENDPOINT_PIPE_vect)
{
	/* Save previously selected endpoint before selecting a new endpoint */
	uint8_t PrevSelectedEndpoint = Endpoint_GetCurrentEndpoint();

	/* Check if mouse endpoint has interrupted */
	if (Endpoint_HasEndpointInterrupted(MOUSE_EPNUM))
	{
		USB_MouseReport_Data_t MouseReportData = {Button: 0, X: 0, Y: 0};
		uint8_t                JoyStatus_LCL   = Joystick_GetStatus();

		if (JoyStatus_LCL & JOY_UP)
		  MouseReportData.Y =  1;
		else if (JoyStatus_LCL & JOY_DOWN)
		  MouseReportData.Y = -1;

		if (JoyStatus_LCL & JOY_RIGHT)
		  MouseReportData.X =  1;
		else if (JoyStatus_LCL & JOY_LEFT)
		  MouseReportData.X = -1;

		if (JoyStatus_LCL & JOY_PRESS)
		  MouseReportData.Button  = (1 << 0);
		  
		if (HWB_GetStatus())
		  MouseReportData.Button |= (1 << 1);

		/* Clear the endpoint IN interrupt flag */
		USB_INT_Clear(ENDPOINT_INT_IN);

		/* Clear the Mouse Report endpoint interrupt and select the endpoint */
		Endpoint_ClearEndpointInterrupt(MOUSE_EPNUM);
		Endpoint_SelectEndpoint(MOUSE_EPNUM);

		/* Write Mouse Report Data */
		Endpoint_Write_Byte(MouseReportData.Button);
		Endpoint_Write_Byte(MouseReportData.X);
		Endpoint_Write_Byte(MouseReportData.Y);
			
		/* Handshake the IN Endpoint - send the data to the host */
		Endpoint_FIFOCON_Clear();
	}

	/* Restore previously selected endpoint */
	Endpoint_SelectEndpoint(PrevSelectedEndpoint);
}
