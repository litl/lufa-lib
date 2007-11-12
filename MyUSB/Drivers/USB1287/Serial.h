/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef SERIAL_H
#define SERIAL_H

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		
		#include "../../Common/FunctionAttributes.h"
		#include "TerminalCodes.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define Serial_IsCharRecieved() (!(UCSR1A & (1 << RXC1)))

		/* Function Prototypes: */
			void Serial_Init(const uint16_t BaudRate);
			void Serial_TxString_P(const char *FlashStringPtr) ATTR_NON_NULL_PTR_ARG(1);
			void Serial_TxString(const char *StringPtr) ATTR_NON_NULL_PTR_ARG(1);
			void Serial_TxByte(const char Data);
			char Serial_RxByte(void);

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define SERIAL_UBBRVAL(baud)    (((F_CPU / (baud * 16UL))) - 1)

#endif
