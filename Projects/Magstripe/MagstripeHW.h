/*
  Copyright 2008  Denver Gingerich (denver [at] ossguy [dot] com)

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
	Demonstration application for a TTL magnetic stripe reader (such as the
	Omron V3B-4K) by Denver Gingerich. See http://ossguy.com/ss_usb/ for the
	demonstration project website, including construction and support details.

	This example is based on the MyUSB Keyboard demonstration application,
	written by Denver Gingerich.
*/

/*
	NOTE: The user of this include file MUST define the following macros
	prior to including the file:

	MAG_DATA_PIN    Pin connected to data wire (ie. PORTC0)
	MAG_CLOCK_PIN   Pin connected to clock wire (ie. PORTC3)
	MAG_CLS_PIN     Pin connected to card loaded wire (ie. PORTC4)
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

#ifndef _MAGSTRIPEHW_H_
#define _MAGSTRIPEHW_H_

	/* Includes: */
		#include <avr/io.h>

		#include <MyUSB/Common/Common.h>

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
				MAG_DDR &= ~(MAG_MASK);
				MAG_PORT |= MAG_MASK;
			};

			static inline uint8_t Magstripe_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Magstripe_GetStatus(void)
			{
				/* Magstripe IOs are active low and must be inverted when read */
				return (~MAG_PIN & MAG_MASK);
			}

#endif
