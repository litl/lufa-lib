/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Driver for the USART subsystem on supported USB AVRs.
 */
 
#ifndef __SERIAL_H__
#define __SERIAL_H__

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#include <stdbool.h>
		
		#include "../../Common/Common.h"
		#include "../Misc/TerminalCodes.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */	
			/** Indicates whether a character has been received through the USART - boolean false if no character
			 *  has been received, or non-zero if a character is waiting to be read from the reception buffer.
			 */
			#define Serial_IsCharRecieved() ((UCSR1A & (1 << RXC1)) ? true : false)

			/** Macro for calculating the baud value from a given baud rate when the U2X (double speed) bit is
			 *  not set.
			 */
			#define SERIAL_UBBRVAL(baud)    (((F_CPU / 16) / baud) - 1)

			/** Macro for calculating the baud value from a given baud rate when the U2X (double speed) bit is
			 *  set.
			 */
			#define SERIAL_2X_UBBRVAL(baud) (((F_CPU / 8) / baud) - 1)

		/* Function Prototypes: */
			/** Initializes the USART, ready for serial data transmission and reception.
			 *
			 *  \param BaudRate  Baud rate to configure the USART at
			 */
			void Serial_Init(const uint16_t BaudRate);

			/** Transmits a given string located in program space (FLASH) through the USART.
			 *
			 *  \param FlashStringPtr  Pointer to a string located in program space
			 */
			void Serial_TxString_P(const char *FlashStringPtr) ATTR_NON_NULL_PTR_ARG(1);

			/** Transmits a given string located in SRAM memory through the USART.
			 *
			 *  \param StringPtr  Pointer to a string located in SRAM space
			 */
			void Serial_TxString(const char *StringPtr) ATTR_NON_NULL_PTR_ARG(1);

		/* Inline Functions: */
			/** Transmits a given byte through the USART.
			 *
			 *  \param DataByte  Byte to transmit through the USART
			 */
			static inline void Serial_TxByte(const char DataByte)
			{
				while (!(UCSR1A & (1 << UDRE1)));
				UDR1 = DataByte;
			}

			/** Receives a byte from the USART.
			 *
			 *  \return Byte received from the USART
			 */
			static inline char Serial_RxByte(void)
			{
				while (!(UCSR1A & (1 << RXC1)));
				return UDR1; 
			}

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
		
#endif
