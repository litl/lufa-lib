/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
	MIDI demonstration application. This gives a simple reference
	application for implementing the USB-MIDI class in USB devices.
	It is built upon the USB Audio class.
	
	Joystick movements are translated into note on/off messages and
	are sent to the host PC as MIDI streams which can be read by any
	MIDI program supporting MIDI IN devices.
	
	If the HWB is not pressed, channel 0 (default piano) is used. If
	the HWB is set, then channel 9 (default percussion) is selected.
*/

#include "MIDI.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,     "MyUSB MIDI App");
BUTTLOADTAG(BuildTime,    __TIME__);
BUTTLOADTAG(BuildDate,    __DATE__);
BUTTLOADTAG(MyUSBVersion, "MyUSB V" MYUSB_VERSION_STRING);

/* Scheduler Task List */
TASK_LIST
{
	{ Task: USB_USBTask          , TaskStatus: TASK_STOP },
	{ Task: USB_MIDI_Task        , TaskStatus: TASK_STOP },
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
	/* Stop running audio and USB management tasks */
	Scheduler_SetTaskMode(USB_MIDI_Task, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDS_LED1 | LEDS_LED3);
}

EVENT_HANDLER(USB_CreateEndpoints)
{
	/* Setup MIDI stream endpoints */
	Endpoint_ConfigureEndpoint(MIDI_STREAM_OUT_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, MIDI_STREAM_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(MIDI_STREAM_IN_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, MIDI_STREAM_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDS_LED2 | LEDS_LED4);

	/* Start audio task */
	Scheduler_SetTaskMode(USB_MIDI_Task, TASK_RUN);
}

TASK(USB_MIDI_Task)
{
	static uint8_t PrevJoystickStatus;

	/* Select the MIDI IN stream */
	Endpoint_SelectEndpoint(MIDI_STREAM_IN_EPNUM);

	/* Check if endpoint is ready to be written to */
	if (Endpoint_ReadWriteAllowed())
	{
		/* Get current joystick mask, XOR with previous to detect joystick changes */
		uint8_t JoystickStatus  = Joystick_GetStatus();
		uint8_t JoystickChanges = (JoystickStatus ^ PrevJoystickStatus);
		
		/* Get HWB status - if set use channel 9 (percussion), otherwise use channel 0 */
		uint8_t Channel = ((HWB_GetStatus()) ? 9 : 0);

		if (JoystickChanges & JOY_LEFT)
		  SendMIDINoteChange(0x3C, (JoystickStatus & JOY_LEFT), 0, Channel);

		if (JoystickChanges & JOY_UP)
		  SendMIDINoteChange(0x3D, (JoystickStatus & JOY_UP), 0, Channel);

		if (JoystickChanges & JOY_RIGHT)
		  SendMIDINoteChange(0x3E, (JoystickStatus & JOY_RIGHT), 0, Channel);

		if (JoystickChanges & JOY_DOWN)
		  SendMIDINoteChange(0x3F, (JoystickStatus & JOY_DOWN), 0, Channel);

		if (JoystickChanges & JOY_PRESS)
		  SendMIDINoteChange(0x3B, (JoystickStatus & JOY_PRESS), 0, Channel);

		/* Save previous joystick value for next joystick change detection */
		PrevJoystickStatus = JoystickStatus;
	}

	/* Select the MIDI OUT stream */
	Endpoint_SelectEndpoint(MIDI_STREAM_OUT_EPNUM);

	/* Check if endpoint is ready to be read from, if so discard its (unused) data */
	if (Endpoint_ReadWriteAllowed())
	  Endpoint_FIFOCON_Clear();
}

void SendMIDINoteChange(const uint8_t Pitch, const bool OnOff, const uint8_t CableID, const uint8_t Channel)
{
	uint8_t Command;

	/* Check if the message should be a Note On or Note Off command */
	Command = ((OnOff)? MIDI_COMMAND_NOTE_ON : MIDI_COMMAND_NOTE_OFF);

	/* Write the Packet Header to the endpoint */
	Endpoint_Write_Byte((CableID << 4) | (Command >> 4));

	/* Write the Note On/Off command with the specified channel, pitch and velocity */
	Endpoint_Write_Byte(Command | Channel);
	Endpoint_Write_Byte(Pitch);
	Endpoint_Write_Byte(MIDI_STANDARD_VELOCITY);
	
	/* Send the data in the endpoint to the host */
	Endpoint_FIFOCON_Clear();
}
