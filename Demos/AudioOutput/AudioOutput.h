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

		#include "Descriptors.h"
		
		#include <MyUSB/Common/ButtLoadTag.h>         // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/Board/Bicolour.h>     // Bicolour LEDs driver
		#include <MyUSB/Scheduler/Scheduler.h>        // Simple scheduler for task management

	/* Preprocessor Checks: */
		#if (!defined(AUDIO_OUT_MONO) && !defined(AUDIO_OUT_STEREO) && !defined(AUDIO_OUT_LEDS))
			#error One of AUDIO_OUT_MONO, AUDIO_OUT_STEREO or AUDIO_OUT_LEDS must be selected.
		#elif ((defined(AUDIO_OUT_MONO)   && (defined(AUDIO_OUT_STEREO) || defined(AUDIO_OUT_LEDS))) || \
		       (defined(AUDIO_OUT_STEREO) && (defined(AUDIO_OUT_MONO)   || defined(AUDIO_OUT_LEDS))) || \
		       (defined(AUDIO_OUT_LEDS)   && (defined(AUDIO_OUT_MONO)   || defined(AUDIO_OUT_STEREO))))
			#error Only one of AUDIO_OUT_MONO, AUDIO_OUT_STEREO or AUDIO_OUT_LEDS must be selected.
		#endif

	/* Task Definitions: */
		TASK(USB_Audio_Task);

	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_CreateEndpoints);
		HANDLES_EVENT(USB_UnhandledControlPacket);

#endif
