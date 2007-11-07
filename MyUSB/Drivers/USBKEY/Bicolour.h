/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef BICOLOUR_H
#define BICOLOUR_H

	/* Includes: */
		#include <avr/io.h>

		#include "../../Common/FunctionAttributes.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define BICOLOUR_LED1_RED    (1 << 4)
			#define BICOLOUR_LED1_GREEN  (1 << 5)
			#define BICOLOUR_LED2_RED    (1 << 7)
			#define BICOLOUR_LED2_GREEN  (1 << 6)
			#define BICOLOUR_LED1_ORANGE (BICOLOUR_LED1_RED | BICOLOUR_LED1_GREEN)
			#define BICOLOUR_LED2_ORANGE (BICOLOUR_LED2_RED | BICOLOUR_LED2_GREEN)
			#define BICOLOUR_ALL_LEDS    (BICOLOUR_LED1_ORANGE | BICOLOUR_LED2_ORANGE)
	
		/* Inline Functions: */
			static inline void Bicolour_Init(void)
			{
				DDRD  |=  BICOLOUR_ALL_LEDS;
				PORTD &= ~BICOLOUR_ALL_LEDS;
			}
			
			static inline void Bicolour_TurnOnLeds(const uint8_t LedMask)
			{
				PORTD |= LedMask;
			}

			static inline void Bicolour_TurnOffLeds(const uint8_t LedMask)
			{
				PORTD &= ~LedMask;
			}

			static inline void Bicolour_SetLeds(const uint8_t LedMask)
			{
				PORTD = ((PORTD & ~BICOLOUR_ALL_LEDS) | LedMask);
			}
			
			static inline void Bicolour_SetLed(const uint8_t LedNumber, const uint8_t LedMask)
			{
				if (LedNumber == 2)
				  PORTD = ((PORTD & ~BICOLOUR_LED2_ORANGE) | (LedMask & BICOLOUR_LED2_ORANGE));
				else if (LedNumber == 1)
				  PORTD = ((PORTD & ~BICOLOUR_LED1_ORANGE) | (LedMask & BICOLOUR_LED1_ORANGE));
			}
			
			static inline uint8_t Bicolour_GetLeds(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Bicolour_GetLeds(void)
			{
				return (PORTD & BICOLOUR_ALL_LEDS);
			}

#endif
