/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  This file is the master dispatch header file for the board-specific HWB driver, for boards containing a
 *  physical pushbutton connected to the AVR's HWB IO pin.
 *
 *  User code should include this file, which will in turn include the correct HWB driver header file for the
 *  currently selected board.
 *
 *  If the BOARD value is set to BOARD_USER, this will include the /Board/HWB.h file in the user project
 *  directory.
 */
 
#ifndef __HWB_H__
#define __HWB_H__

	/* Macros: */
	#if !defined(__DOXYGEN__)
		#define INCLUDE_FROM_HWB_H
		#define INCLUDE_FROM_BOARD_DRIVER
	#endif

	/* Includes: */
	#include "../../Common/BoardTypes.h"
	
	#if !defined(BOARD)
		#error BOARD must be set in makefile to BOARD_USBKEY, BOARD_STK525, BOARD_STK526, BOARD_RZUSBSTICK or BOARD_USER.	
	#elif (BOARD == BOARD_USBKEY)
		#include "USBKEY/HWB.h"
	#elif (BOARD == BOARD_STK525)
		#include "STK525/HWB.h"
	#elif (BOARD == BOARD_STK526)
		#include "STK526/HWB.h"
	#elif (BOARD == BOARD_RZUSBSTICK)
		#error The selected board does not contain a HWB.
	#elif (BOARD == BOARD_USER)
		#include "Board/HWB.h"		
	#endif
	
	/* Psudo-Functions for Doxygen: */
	#if defined(__DOXYGEN__)
		/** Initializes the HWB driver, so that the current button position can be read. This sets the appropriate
		 *  I/O pin to an input with pull-up enabled.
		 *
		 *  This must be called before any HWB functions are used.
		 */
		static inline void HWB_Init(void);
		
		/** Returns the current position of the HWB button on the board.
		 *
		 *  \return Boolean true if the button is currently pressed, false otherwise
		 */
		static inline bool HWB_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
	#endif

#endif
