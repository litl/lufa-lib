/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Macros and functions for TTL magnetic stripe readers (such as the Omron
	V3B-4K) added by Denver Gingerich.

	This file is based on the MyUSB joystick macros and functions,
	written by Dean Camera.
*/

/*
	NOTE: The user of this include file MUST define the following macros
	prior to including the file:

	MAG_DATA_PIN    pin connected to data wire (ie. PORTC0)
	MAG_CLOCK_PIN   pin connected to clock wire (ie. PORTC3)
	MAG_CLS_PIN     pin connected to card loaded wire (ie. PORTC4)
	MAG_PIN         PIN macro for the reader's port (ie. PINC)
	MAG_DDR         DDR macro for the reader's port (ie. DDRC)
	MAG_PORT        PORT macro for the reader's port (ie. PORTC)

	The example macros listed above assumed that the data wire is connected
	to pin 0 on port C, the clock wire is connected to pin 3 on port C, and
	the card loaded wire is connected to pin 4 on port C.

	Connecting wires to pins on different ports (ie. data wire to pin 0 on
	port C and clock wire to pin 0 on port D) is currently unsupported.  All
	wires must be connected to the same port.
*/

#ifndef __MAGSTRIPE_H__
#define __MAGSTRIPE_H__

	/* Includes: */
		#include <avr/io.h>

		#include "../../Common/FunctionAttributes.h"

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define MAG_MASK    (MAG_DATA | MAG_CLOCK | MAG_CLS)

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define MAG_DATA    (1 << MAG_DATA_PIN)
			#define MAG_CLOCK   (1 << MAG_CLOCK_PIN)
			#define MAG_CLS     (1 << MAG_CLS_PIN)

		/* Inline Functions: */
			static inline void Magstripe_Init(void)
			{
				/* Magstripe Inputs */
				MAG_DDR &= ~(MAG_MASK);

				/* Turn on Magstripe pullups */
				MAG_PORT |= MAG_MASK;
			};

			static inline uint8_t Magstripe_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Magstripe_GetStatus(void)
			{
				/* Magstripe IOs are active low and must be inverted when read */
				return (~MAG_PIN & MAG_MASK);
			}

#endif
