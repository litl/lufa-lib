#ifndef JOYSTICK_H
#define JOYSTICK_H

	/* Includes */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>
		
		#include "../../Common/ISRMacro.h"
		#include "../../Common/FunctionAttributes.h"

	/* Public Macros */
		#define JOY_LEFT                  (1 << 6)
		#define JOY_RIGHT                 (1 << 3)
		#define JOY_UP                    (1 << 7)
		#define JOY_DOWN                  (1 << 4)
		#define JOY_PRESS                 (1 << 5)
	
	/* Private Macros */
		#define JOY_BMASK                 ((1 << 5) | (1 << 6) | (1 << 7))
		#define JOY_EMASK                 ((1 << 4) | (1 << 5))
		
	/* Inline Functions */
		static inline void Joystick_Init(void)
		{
			/* Joystick Inputs */
			DDRB &= ~(JOY_BMASK);
			DDRE &= ~(JOY_EMASK);

			/* Turn on Joystick pullups */
			PORTB |= JOY_BMASK;
			PORTE |= JOY_EMASK;				
		};
		
		static inline uint8_t Joystick_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
		static inline uint8_t Joystick_GetStatus(void)
		{
			return (~PINB & JOY_BMASK) | ((~PINE & JOY_EMASK) >> 1);		
		}

#endif
