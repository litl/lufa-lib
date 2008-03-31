/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __LEDS_H__
#define __LEDS_H__

	/* Macros: */
	#define INCLUDE_FROM_LEDS_H
	#define INCLUDE_FROM_BOARD_DRIVER

	/* Includes: */
	#include "../../Common/BoardTypes.h"

	#if !defined(BOARD)
		#error BOARD must be set in makefile to BOARD_USBKEY, BOARD_STK525, BOARD_STK526, BOARD_RZUSBSTICK or BOARD_USER.
	#elif (BOARD == BOARD_USBKEY)
		#include "USBKEY/LEDs.h"
	#elif (BOARD == BOARD_STK525)
		#include "STK525/LEDs.h"
	#elif (BOARD == BOARD_STK526)
		#include "STK526/LEDs.h"
	#elif (BOARD == BOARD_RZUSBSTICK)
		#include "RZUSBSTICK/LEDs.h"
	#elif (BOARD == BOARD_USER)
		#include "Board/LEDs.h"
	#endif

#endif
