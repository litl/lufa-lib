/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  This file is the master dispatch header file for the board-specific Joystick driver, for boards containing a
 *  5-way joystick.
 *
 *  User code should include this file, which will in turn include the correct joystick driver header file for the
 *  currently selected board.
 *
 *  If the BOARD value is set to BOARD_USER, this will include the /Board/Joystick.h file in the user project
 *  directory.
 */
 
#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

	/* Macros: */
	#if !defined(__DOXYGEN__)
		#define INCLUDE_FROM_JOYSTICK_H
		#define INCLUDE_FROM_BOARD_DRIVER
	#endif

	/* Includes: */
	#include "../../Common/Common.h"
	
	#if !defined(BOARD)
		#error BOARD must be set in makefile to BOARD_USBKEY, BOARD_STK525, BOARD_STK526, BOARD_RZUSBSTICK or BOARD_USER.	
	#elif (BOARD == BOARD_USBKEY)
		#include "USBKEY/Joystick.h"
	#elif (BOARD == BOARD_STK525)
		#include "STK525/Joystick.h"
	#elif (BOARD == BOARD_STK526)
		#include "STK526/Joystick.h"
	#elif (BOARD == BOARD_RZUSBSTICK)
		#error The selected board does not contain a joystick.
	#elif (BOARD == BOARD_USER)
		#include "Board/Joystick.h"		
	#endif

	/* Psudo-Functions for Doxygen: */
	#if defined(__DOXYGEN__)
		/** Initializes the joystick driver so that the joystick position can be read. This sets the appropriate
		 *  I/O pins to inputs with their pull-ups enabled.
		 */
		static inline void Joystick_Init(void);

		/** Returns the current status of the joystick, as a mask indicating the direction the joystick is
		 *  currently facing in (multiple bits can be set).
		 *
		 *  \return Mask indicating the joystick direction - see corresponding board specific Joystick.h file
		 *          for direction masks
		 */
		static inline uint8_t Joystick_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
	#endif

#endif
