/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __LEDS_RZUSBSTICK_H__
#define __LEDS_RZUSBSTICK_H__

	/* Includes: */
		#include <avr/io.h>

		#include "../../../Common/Common.h"

	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_LEDS_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/LEDS.h instead.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define LEDS_LED1             (1 << 7)
			#define LEDS_LED2             (1 << 5)
			#define LEDS_LED3             ((1 << 6) >> LEDS_PORTE_MASK_SHIFT)
			#define LEDS_LED4             ((1 << 7) >> LEDS_PORTE_MASK_SHIFT)
			#define LEDS_ALL_LEDS         (LEDS_LED1 | LEDS_LED2 | LEDS_LED3 | LEDS_LED4)
			#define LEDS_NO_LEDS          0
			
	/* Private Interface - For use in library only: */
		/* Macros: */
			#define LEDS_PORTD_LEDS       (LEDS_LED1 | LEDS_LED2)
			#define LEDS_PORTE_LEDS       (LEDS_LED3 | LEDS_LED4)
			
			#define LEDS_PORTE_MASK_SHIFT 4
	
		/* Inline Functions: */
			static inline void LEDs_Init(void)
			{
				DDRD  |=  LEDS_PORTD_LEDS;
				PORTD &= ~LEDS_LED1;
				PORTD |=  LEDS_LED2;

				DDRE  |=  (LEDS_PORTE_LEDS << LEDS_PORTE_MASK_SHIFT);
				PORTE |=  (LEDS_PORTE_LEDS << LEDS_PORTE_MASK_SHIFT);
			}
			
			static inline void LEDs_TurnOnLEDs(const uint8_t LEDMask)
			{
				PORTD |=  (LEDMask & LEDS_LED1);
				PORTD &= ~(LEDMask & LEDS_LED2);			
				PORTE &= ~((LEDMask & LEDS_PORTE_LEDS) << LEDS_PORTE_MASK_SHIFT);
			}

			static inline void LEDs_TurnOffLEDs(const uint8_t LEDMask)
			{
				PORTD &= ~(LEDMask & LEDS_LED1);
				PORTD |=  (LEDMask & LEDS_LED2);
				PORTE |=  ((LEDMask & LEDS_PORTE_LEDS) << LEDS_PORTE_MASK_SHIFT);
			}

			static inline void LEDs_SetAllLEDs(const uint8_t LEDMask)
			{
				PORTD = (((PORTD & ~LEDS_LED1) |  (LEDMask & LEDS_LED1)) |
				         ((PORTD |  LEDS_LED2) & ~(LEDMask & LEDS_LED2)));
				PORTE = ((PORTE | (LEDS_PORTE_LEDS << LEDS_PORTE_MASK_SHIFT)) &
				        ~((LEDMask & LEDS_PORTE_LEDS) << LEDS_PORTE_MASK_SHIFT));
			}
			
			static inline void LEDs_ChangeLEDs(const uint8_t LEDMask, const uint8_t ActiveMask)
			{
				PORTD = (((PORTD & ~(LEDMask & LEDS_LED1)) |  (ActiveMask & LEDS_LED1)) |
				         ((PORTD |  (LEDMask & LEDS_LED2)) & ~(ActiveMask & LEDS_LED2)));
				PORTE = ((PORTE | ((LEDMask & LEDS_PORTE_LEDS) << LEDS_PORTE_MASK_SHIFT)) &
				        ~((ActiveMask & LEDS_PORTE_LEDS) << LEDS_PORTE_MASK_SHIFT));
			}
			
			static inline uint8_t LEDs_GetLEDs(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t LEDs_GetLEDs(void)
			{
				return (((PORTD & LEDS_LED1) | (~PORTD & LEDS_LED2)) |
				        ((~PORTE & (LEDS_PORTE_LEDS << LEDS_PORTE_MASK_SHIFT)) >> LEDS_PORTE_MASK_SHIFT));
			}

#endif
