/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __BICOLOUR_H__
#define __BICOLOUR_H__

	/* Macros: */
	#define INCLUDE_FROM_BICOLOUR_H
	#define INCLUDE_FROM_BOARD_DRIVER

	/* Includes: */
	#include "../../Common/BoardTypes.h"

	#if !defined(BOARD)
		#error BOARD must be set in makefile to BOARD_USBKEY, BOARD_STK525, BOARD_STK526 or BOARD_USER.	
	#elif (BOARD == BOARD_USBKEY)
		#include "USBKEY/Bicolour.h"
	#elif (BOARD == BOARD_STK525)
		#include "STK525/Bicolour.h"
	#elif (BOARD == BOARD_STK526)
		#include "STK526/Bicolour.h"
	#elif (BOARD == BOARD_USER)
		#include "Board/Bicolour.h"
	#endif
	
	/* Public Interface - May be used in end-application: */
		/* Inline Functions: */
			#if (BOARD != BOARD_USER)
				static inline void Bicolour_Init(void)
				{
					BICOLOUR_DDR  |=  BICOLOUR_ALL_LEDS;
					BICOLOUR_PORT &= ~BICOLOUR_ALL_LEDS;
				}
				
				static inline void Bicolour_TurnOnLeds(const uint8_t LedMask)
				{
					BICOLOUR_PORT |= LedMask;
				}

				static inline void Bicolour_TurnOffLeds(const uint8_t LedMask)
				{
					BICOLOUR_PORT &= ~LedMask;
				}

				static inline void Bicolour_SetLeds(const uint8_t LedMask)
				{
					BICOLOUR_PORT = ((BICOLOUR_PORT & ~BICOLOUR_ALL_LEDS) | LedMask);
				}
				
				static inline void Bicolour_SetLed(const uint8_t LedNumber, const uint8_t LedMask)
				{
					if (LedNumber == 2)
					  BICOLOUR_PORT = ((BICOLOUR_PORT & ~BICOLOUR_LED2_ORANGE) | (LedMask & BICOLOUR_LED2_ORANGE));
					else if (LedNumber == 1)
					  BICOLOUR_PORT = ((BICOLOUR_PORT & ~BICOLOUR_LED1_ORANGE) | (LedMask & BICOLOUR_LED1_ORANGE));
				}
				
				static inline uint8_t Bicolour_GetLeds(void) ATTR_WARN_UNUSED_RESULT;
				static inline uint8_t Bicolour_GetLeds(void)
				{
					return (BICOLOUR_PORT & BICOLOUR_ALL_LEDS);
				}
			#endif

#endif
