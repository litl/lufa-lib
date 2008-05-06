/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
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
		HANDLES_EVENT(USB_CreateEndpoints);

   /* Function Prototypes: */
   	void SendMIDINoteChange(const uint8_t Pitch, const bool OnOff, const uint8_t CableID, const uint8_t Channel);

#endif
