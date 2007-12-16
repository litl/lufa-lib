/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef _AUDIO_OUTPUT_H_
#define _AUDIO_OUTPUT_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>

		#include "Descriptors.h"
		
		#include <MyUSB/Drivers/USB1287/Serial_Stream.h>
		
		#include <MyUSB/Common/ButtLoadTag.h>         // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/USBKEY/Joystick.h>    // Joystick driver for the USBKEY
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>    // Bicolour LEDs driver for the USBKEY
		#include <MyUSB/Scheduler/Scheduler.h>        // Simple scheduler for task management
		
	/* Macros: */
		#define GET_CUR                        0x81
		#define GET_MAX                        0x83
		#define GET_MIN                        0x82
		#define GET_RES                        0x84
		
		#define SET_CUR                        0x01
		
		#define GET_SET_MUTE                   0x0100
		#define GET_SET_VOLUME                 0x0200
		
		#define VOL_MIN                        (int16_t)0x8000
		#define VOL_MAX                        (int16_t)0x7FFF
		#define VOL_RES                        0x000A
		#define VOL_SILENCE                    0x8001
		
	/* Preprocessor Checks: */
		#if (!defined(AUDIO_OUT_MONO) && !defined(AUDIO_OUT_STEREO) && !defined(AUDIO_OUT_LEDS))
			#error One of AUDIO_OUT_MONO, AUDIO_OUT_STEREO or AUDIO_OUT_LEDS must be selected.
		#elif ((defined(AUDIO_OUT_MONO) && (defined(AUDIO_OUT_STEREO) || defined(AUDIO_OUT_LEDS))) || \
		       (defined(AUDIO_OUT_STEREO) && (defined(AUDIO_OUT_MONO) || defined(AUDIO_OUT_LEDS))) || \
		       (defined(AUDIO_OUT_LEDS) && (defined(AUDIO_OUT_MONO) || defined(AUDIO_OUT_STEREO))))
			#error Only one of AUDIO_OUT_MONO, AUDIO_OUT_STEREO or AUDIO_OUT_LEDS must be selected.
		#endif

	/* Task Definitions: */
		TASK(USB_Audio_Task);

	/* Event Handlers: */
		HANDLES_EVENT(USB_CreateEndpoints);
		HANDLES_EVENT(USB_UnhandledControlPacket);

#endif
