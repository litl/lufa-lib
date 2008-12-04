/*
             LUFA Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/*
	MIDI demonstration application. This gives a simple reference
	application for implementing the USB-MIDI class in USB devices.
	It is built upon the USB Audio class.
	
	Joystick movements are translated into note on/off messages and
	are sent to the host PC as MIDI streams which can be read by any
	MIDI program supporting MIDI IN devices.
	
	If the HWB is not pressed, channel 1 (default piano) is used. If
	the HWB is set, then channel 10 (default percussion) is selected.
	
	This device implements MIDI-THRU mode, with the IN MIDI data being
	generated by the device itself. OUT MIDI data is discarded.
*/

/*
	USB Mode:           Device
	USB Class:          Audio Class
	USB Subclass:       MIDI Audio Device
	Relevant Standards: USBIF Audio Class Specification
	                    USB-MIDI Audio Class Extention Specification
	                    General MIDI Specification
	Usable Speeds:      Full Speed Mode
*/

#include "MIDI.h"

/* Project Tags, for reading out using the ButtLoad project */
BUTTLOADTAG(ProjName,    "LUFA MIDI App");
BUTTLOADTAG(BuildTime,   __TIME__);
BUTTLOADTAG(BuildDate,   __DATE__);
BUTTLOADTAG(LUFAVersion, "LUFA V" LUFA_VERSION_STRING);

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
	UpdateStatus(Status_USBNotReady);
	
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
	UpdateStatus(Status_USBEnumerating);
}

EVENT_HANDLER(USB_Disconnect)
{
	/* Stop running audio and USB management tasks */
	Scheduler_SetTaskMode(USB_MIDI_Task, TASK_STOP);
	Scheduler_SetTaskMode(USB_USBTask, TASK_STOP);

	/* Indicate USB not ready */
	UpdateStatus(Status_USBNotReady);
}

EVENT_HANDLER(USB_ConfigurationChanged)
{
	/* Setup MIDI stream endpoints */
	Endpoint_ConfigureEndpoint(MIDI_STREAM_OUT_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_OUT, MIDI_STREAM_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(MIDI_STREAM_IN_EPNUM, EP_TYPE_BULK,
		                       ENDPOINT_DIR_IN, MIDI_STREAM_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	/* Indicate USB connected and ready */
	UpdateStatus(Status_USBReady);

	/* Start MIDI task */
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
		
		/* Get HWB status - if set use channel 10 (percussion), otherwise use channel 1 */
		uint8_t Channel = ((HWB_GetStatus()) ? MIDI_CHANNEL(10) : MIDI_CHANNEL(1));

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
	  Endpoint_ClearCurrentBank();
}

/** Function to manage status updates to the user. This is done via LEDs on the given board, if available, but may be changed to
 *  log to a serial port, or anything else that is suitable for status updates.
 *
 *  \param CurrentStatus  Current status of the system, from the StatusCodes_t enum
 */
void UpdateStatus(uint8_t CurrentStatus)
{
	uint8_t LEDMask = LEDS_NO_LEDS;
	
	/* Set the LED mask to the appropriate LED mask based on the given status code */
	switch (CurrentStatus)
	{
		case Status_USBNotReady:
			LEDMask = (LEDS_LED1);
			break;
		case Status_USBEnumerating:
			LEDMask = (LEDS_LED1 | LEDS_LED2);
			break;
		case Status_USBReady:
			LEDMask = (LEDS_LED2 | LEDS_LED4);
			break;
	}
	
	/* Set the board LEDs to the new LED mask */
	LEDs_SetAllLEDs(LEDMask);
}

void SendMIDINoteChange(const uint8_t Pitch, const bool OnOff, const uint8_t CableID, const uint8_t Channel)
{
	/* Wait until endpoint ready for more data */
	while (!(Endpoint_ReadWriteAllowed()));

	/* Check if the message should be a Note On or Note Off command */
	uint8_t Command = ((OnOff)? MIDI_COMMAND_NOTE_ON : MIDI_COMMAND_NOTE_OFF);

	/* Write the Packet Header to the endpoint */
	Endpoint_Write_Byte((CableID << 4) | (Command >> 4));

	/* Write the Note On/Off command with the specified channel, pitch and velocity */
	Endpoint_Write_Byte(Command | Channel);
	Endpoint_Write_Byte(Pitch);
	Endpoint_Write_Byte(MIDI_STANDARD_VELOCITY);
	
	/* Send the data in the endpoint to the host */
	Endpoint_ClearCurrentBank();
}
