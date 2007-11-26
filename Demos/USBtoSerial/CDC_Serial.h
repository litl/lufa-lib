/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __CDC_SERIAL_H__
#define __CDC_SERIAL_H__

	/* Includes: */
		#include <avr/io.h>
		
		#include "USBtoSerial.h"
		
	/* Macros: */
		#define SERIAL_2X_UBBRVAL(baud)    (((F_CPU / 8) / baud) - 1)

	/* Function Prototypes: */
		void CDC_Serial_Init(void);

#endif
