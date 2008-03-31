/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __ADC_H__
#define __ADC_H__

	/* Macros: */
	#define INCLUDE_FROM_ADC_H
	#define INCLUDE_FROM_CHIP_DRIVER

	/* Includes: */
		#if (defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__) || \
		     defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB647__) || \
			 defined(__AVR_ATmega32U4__))
			#include "AT90USBXXX67/ADC.h"
		#else
			#error "ADC is not avaliable for the currently selected AVR model."
		#endif

#endif
