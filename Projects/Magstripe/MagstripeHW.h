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

	MAG_T1_CLOCK_PIN   Pin connected to Track 1 clock wire (ie. PORTC1)
	MAG_T1_DATA_PIN    Pin connected to Track 1 data wire (ie. PORTC2)
	MAG_T2_CLOCK_PIN   Pin connected to Track 2 clock wire (ie. PORTC3)
	MAG_T2_DATA_PIN    Pin connected to Track 2 data wire (ie. PORTC0)
	MAG_T3_CLOCK_PIN   Pin connected to Track 3 clock wire (ie. PORTC5)
	MAG_T3_DATA_PIN    Pin connected to Track 3 data wire (ie. PORTC6)
	MAG_CLS_PIN        Pin connected to card loaded wire (ie. PORTC4)
	MAG_PIN            PIN macro for the reader's port (ie. PINC)
	MAG_DDR            DDR macro for the reader's port (ie. DDRC)
	MAG_PORT           PORT macro for the reader's port (ie. PORTC)

	The example macros listed above assume that the Track 2 data wire is
	connected to pin 0 on port C, the Track 2 clock wire is connected to
	pin 3 on port C (similarly for Tracks 1 and 3), and the card loaded
	wire is connected to pin 4 on port C.

	If the magstripe reader you are using only reads one or two tracks,
	then set the clock and data pins for the tracks it doesn't read to a
	pin that is unused.  For example, on the AT90USBKey, any of the pins on
	port C that do not have wires attached will be unused since they are
	not connected to any onboard devices (such as the joystick or
	temperature sensor).

	Connecting wires to pins on different ports (ie. a data wire to pin 0
	on port C and a clock wire to pin 0 on port D) is currently
	unsupported.  All pins specified above must be on the same port.
*/

#ifndef _MAGSTRIPEHW_H_
#define _MAGSTRIPEHW_H_

	/* Includes: */
		#include <avr/io.h>

		#include <MyUSB/Common/Common.h>

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define MAG_MASK    (MAG_T1_DATA | MAG_T1_CLOCK | \
					     MAG_T2_DATA | MAG_T2_CLOCK | \
					     MAG_T3_DATA | MAG_T3_CLOCK | \
					     MAG_CLS)

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define MAG_T1_DATA    (1 << MAG_T1_DATA_PIN)
			#define MAG_T1_CLOCK   (1 << MAG_T1_CLOCK_PIN)
			#define MAG_T2_DATA    (1 << MAG_T2_DATA_PIN)
			#define MAG_T2_CLOCK   (1 << MAG_T2_CLOCK_PIN)
			#define MAG_T3_DATA    (1 << MAG_T3_DATA_PIN)
			#define MAG_T3_CLOCK   (1 << MAG_T3_CLOCK_PIN)
			#define MAG_CLS        (1 << MAG_CLS_PIN)

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
