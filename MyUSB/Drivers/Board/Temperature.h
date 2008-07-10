/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Temperature sensor board driver for the USB boards which contain a temperature sensor.
 */


#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

	/* Macros: */
	#if !defined(__DOXYGEN__)
		#define INCLUDE_FROM_BOARD_DRIVER
	#endif
	
	/* Includes: */
		#include <avr/pgmspace.h>

		#include "../AT90USBXXX/ADC.h"
		#include "../../Common/Common.h"
	
		#if !defined(BOARD)
			#error BOARD must be set in makefile to BOARD_USBKEY, BOARD_STK525, BOARD_STK526, BOARD_RZUSBSTICK or BOARD_USER.	
		#elif (BOARD == BOARD_RZUSBSTICK)
			#error The selected board does not contain a temperature sensor.
		#endif

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** ADC channel number for the temperature sensor. */
			#define TEMP_ADC_CHANNEL   0
			
			/** Minimum returnable temperature from the Temperature_GetTemperature() function. */
			#define TEMP_MIN_TEMP      TEMP_TABLE_OFFSET

			/** Maximum returnable temperature from the Temperature_GetTemperature() function. */
			#define TEMP_MAX_TEMP      ((TEMP_TABLE_SIZE - 1) + TEMP_TABLE_OFFSET)
			
			/** Initializes the temperature sensor driver, including setting up the appropriate ADC channel.
			 *  This must be called before any other temperature sensor routines.
			 *
			 *  The ADC itself (not the ADC channel) must be configured seperately before calling the temperature
			 *  sensor functions.
			 */
			#define Temperature_Init() ADC_SetupChannel(TEMP_ADC_CHANNEL);

		/* Function Prototypes: */
			/** Performs a complete ADC on the temperature sensor channel, and converts the result into a
			 *  valid temperature between TEMP_MIN_TEMP and TEMP_MAX_TEMP in degrees Celcius.
			 *
			 *  \return Signed temperature in degrees Celcius
			 */
			int8_t Temperature_GetTemperature(void) ATTR_WARN_UNUSED_RESULT;

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define TEMP_TABLE_SIZE   (sizeof(Temperature_Lookup) / sizeof(Temperature_Lookup[0]))
			#define TEMP_TABLE_OFFSET -20
	#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
		
#endif
