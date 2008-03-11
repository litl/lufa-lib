/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __JOYSTICK_USBKEY_H__
#define __JOYSTICK_USBKEY_H__

	/* Includes: */
		#include <avr/io.h>

		#include "../../../Common/Common.h"

	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_JOYSTICK_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/Joystick.h instead.
		#endif

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define JOY_BMASK                 ((1 << 5) | (1 << 6) | (1 << 7))
			#define JOY_EMASK                 ((1 << 4) | (1 << 5))

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define JOY_LEFT                  (1 << 6)
			#define JOY_RIGHT                 (1 << 3)
			#define JOY_UP                    (1 << 4)
			#define JOY_DOWN                  (1 << 7)
			#define JOY_PRESS                 (1 << 5)
			
		/* Inline Functions: */
			static inline void Joystick_Init(void)
			{
				DDRB  &= ~(JOY_BMASK);
				DDRE  &= ~(JOY_EMASK);

				PORTB |= JOY_BMASK;
				PORTE |= JOY_EMASK;				
			};
			
			static inline uint8_t Joystick_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Joystick_GetStatus(void)
			{
				return ((~PINB & JOY_BMASK) | ((~PINE & JOY_EMASK) >> 1));
			}

#endif
