/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __BICOLOUR_STK526_H__
#define __BICOLOUR_STK526_H__

	/* Includes: */
		#include <avr/io.h>

		#include "../../../Common/Common.h"

	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_BICOLOUR_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/Bicolour.h instead.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define BICOLOUR_LED1        1
			#define BICOLOUR_LED2        2
		
			#define BICOLOUR_LED1_OFF    0
			#define BICOLOUR_LED2_OFF    0
			#define BICOLOUR_LED1_RED    (1 << 1)
			#define BICOLOUR_LED1_GREEN  (1 << 0)
			#define BICOLOUR_LED2_RED    (1 << 5)
			#define BICOLOUR_LED2_GREEN  (1 << 4)
			#define BICOLOUR_LED1_ORANGE (BICOLOUR_LED1_RED | BICOLOUR_LED1_GREEN)
			#define BICOLOUR_LED2_ORANGE (BICOLOUR_LED2_RED | BICOLOUR_LED2_GREEN)
			#define BICOLOUR_ALL_LEDS    (BICOLOUR_LED1_ORANGE | BICOLOUR_LED2_ORANGE)
			#define BICOLOUR_NO_LEDS     0

#endif
