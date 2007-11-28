/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __SERIAL_H__
#define __SERIAL_H__

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		
		#include "../../Common/FunctionAttributes.h"
		#include "../Misc/TerminalCodes.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define Serial_IsCharRecieved() (!(UCSR1A & (1 << RXC1)))

		/* Function Prototypes: */
			void Serial_Init(const uint16_t BaudRate);
			void Serial_TxString_P(const char *FlashStringPtr) ATTR_NON_NULL_PTR_ARG(1);
			void Serial_TxString(const char *StringPtr) ATTR_NON_NULL_PTR_ARG(1);

		/* Inline Functions: */
			static inline void Serial_TxByte(const char DataByte)
			{
				while (!(UCSR1A & (1 << UDRE1)));
				UDR1 = DataByte;
			}

			static inline char Serial_RxByte(void)
			{
				while (!(UCSR1A & (1 << RXC1)));
				return UDR1; 
			}

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define SERIAL_UBBRVAL(baud)    (((F_CPU / 16) / baud) - 1)

#endif
