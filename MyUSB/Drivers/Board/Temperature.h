/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

	/* Includes: */
		#include <avr/pgmspace.h>

		#include "../AT90USBXXX/ADC.h"
		#include "../../Common/Common.h"
		#include "../../Common/BoardTypes.h"
	
		#if !defined(BOARD)
			#error BOARD must be set in makefile to BOARD_USBKEY, BOARD_STK525, BOARD_STK526, BOARD_RZUSBSTICK or BOARD_USER.	
		#elif (BOARD == BOARD_RZUSBSTICK)
			#error The selected board does not contain a temperature sensor.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define TEMP_ADC_CHANNEL   0
			#define TEMP_MIN_TEMP      TEMP_TABLE_OFFSET
			#define TEMP_MAX_TEMP      ((TEMP_TABLE_SIZE - 1) + TEMP_TABLE_OFFSET)
			
			#define Temperature_Init() ADC_SetupChannel(TEMP_ADC_CHANNEL);

		/* Function Prototypes: */
			int8_t Temperature_GetTemperature(void) ATTR_WARN_UNUSED_RESULT;

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define TEMP_TABLE_SIZE   (sizeof(Temperature_Lookup) / sizeof(Temperature_Lookup[0]))
			#define TEMP_TABLE_OFFSET -20		

#endif
