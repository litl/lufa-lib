/*
             MyUSB Library
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

#ifndef _AUDIO_OUTPUT_H_
#define _AUDIO_OUTPUT_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <stdbool.h>

		#include "Descriptors.h"
				
		#include <MyUSB/Version.h>                            // Library Version Information
		#include <MyUSB/Common/ButtLoadTag.h>                 // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>                    // USB Functionality
		#include <MyUSB/Drivers/Board/Joystick.h>             // Joystick driver
		#include <MyUSB/Drivers/Board/LEDs.h>                 // LEDs driver
		#include <MyUSB/Drivers/Board/HWB.h>                  // Hardware Button driver
		#include <MyUSB/Scheduler/Scheduler.h>                // Simple scheduler for task management

   /* Macros: */
		#define MIDI_COMMAND_NOTE_ON         0x90
		#define MIDI_COMMAND_NOTE_OFF        0x80

		#define MIDI_STANDARD_VELOCITY       64
		
		#define MIDI_CHANNEL(channel)        (channel - 1)

	/* Task Definitions: */
		TASK(USB_MIDI_Task);

	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_ConfigurationChanged);

   /* Function Prototypes: */
		void SendMIDINoteChange(const uint8_t Pitch, const bool OnOff,
		                        const uint8_t CableID, const uint8_t Channel);

#endif
