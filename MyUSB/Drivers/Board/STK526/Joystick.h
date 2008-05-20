/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Board specific joystick driver header for the STK526.
 *
 *  \note This file should not be included directly. It is automatically included as needed by the joystick driver
 *        dispatch header located in MyUSB/Drivers/Board/Joystick.h.
 */
 
#ifndef __JOYSTICK_STK526_H__
#define __JOYSTICK_STK526_H__

	/* Includes: */
		#include <avr/io.h>

		#include "../../../Common/Common.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_JOYSTICK_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/Joystick.h instead.
		#endif

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define JOY_BMASK                 ((1 << 0) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7))
	#endif
	
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Mask for the joystick being pushed in the left direction. */
			#define JOY_LEFT                  (1 << 4)

			/** Mask for the joystick being pushed in the right direction. */
			#define JOY_RIGHT                 (1 << 6)

			/** Mask for the joystick being pushed in the upward direction. */
			#define JOY_UP                    (1 << 5)

			/** Mask for the joystick being pushed in the downward direction. */
			#define JOY_DOWN                  (1 << 7)

			/** Mask for the joystick being pushed inward. */
			#define JOY_PRESS                 (1 << 0)
			
		/* Inline Functions: */
		#if !defined(__DOXYGEN__)
			static inline void Joystick_Init(void)
			{
				DDRB  &= ~(JOY_BMASK);

				PORTB |= JOY_BMASK;
			};
			
			static inline uint8_t Joystick_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Joystick_GetStatus(void)
			{
				return (~PINB & JOY_BMASK);
			}
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
	
#endif
