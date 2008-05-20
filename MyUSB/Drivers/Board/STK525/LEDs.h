/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Board specific LED driver header for the STK525.
 *
 *  \note This file should not be included directly. It is automatically included as needed by the LEDs driver
 *        dispatch header located in MyUSB/Drivers/Board/LEDs.h.
 */
 
#ifndef __LEDS_STK525_H__
#define __LEDS_STK525_H__

	/* Includes: */
		#include <avr/io.h>

		#include "../../../Common/Common.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif
		
	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_LEDS_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/LEDS.h instead.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** LED mask for the first LED on the board. */
			#define LEDS_LED1        (1 << 4)

			/** LED mask for the second LED on the board. */
			#define LEDS_LED2        (1 << 5)

			/** LED mask for the third LED on the board. */
			#define LEDS_LED3        (1 << 7)

			/** LED mask for the fourth LED on the board. */
			#define LEDS_LED4        (1 << 6)

			/** LED mask for all the LEDs on the board. */
			#define LEDS_ALL_LEDS    (LEDS_LED1 | LEDS_LED2 | LEDS_LED3 | LEDS_LED4)

			/** LED mask for the none of the board LEDs */
			#define LEDS_NO_LEDS          0

		/* Inline Functions: */
		#if !defined(__DOXYGEN__)
			static inline void LEDs_Init(void)
			{
				DDRD  |=  LEDS_ALL_LEDS;
				PORTD &= ~LEDS_ALL_LEDS;
			}
			
			static inline void LEDs_TurnOnLEDs(const uint8_t LedMask)
			{
				PORTD |= LedMask;
			}

			static inline void LEDs_TurnOffLEDs(const uint8_t LedMask)
			{
				PORTD &= ~LedMask;
			}

			static inline void LEDs_SetAllLEDs(const uint8_t LedMask)
			{
				PORTD = ((PORTD & ~LEDS_ALL_LEDS) | LedMask);
			}
			
			static inline void LEDs_ChangeLEDs(const uint8_t LedMask, const uint8_t ActiveMask)
			{
				PORTD = ((PORTD & ~LedMask) | ActiveMask);
			}
			
			static inline uint8_t LEDs_GetLEDs(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t LEDs_GetLEDs(void)
			{
				return (PORTD & LEDS_ALL_LEDS);
			}
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
	
#endif
