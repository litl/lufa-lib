/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Serial stream driver for the USART subsystem on supported USB AVRs. This makes use of the functions in the
 *  regular USART driver, but allows the avr-libc standard stream functions (printf, puts, etc.) to work with the
 *  USART.
 **/

#ifndef __SERIAL_STREAM_H__
#define __SERIAL_STREAM_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdio.h>
		
		#include "Serial.h"
	
	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Private Interface - For use in library only: */	
	#if !defined(__DOXYGEN__)
		/* External Variables: */
			extern FILE USARTStream;

		/* Function Prototypes: */
			int SerialStream_TxByte(char DataByte, FILE *Stream) ATTR_NON_NULL_PTR_ARG(2);
			int SerialStream_RxByte(FILE *Stream) ATTR_NON_NULL_PTR_ARG(1);
	#endif

	/* Public Interface - May be used in end-application: */
		/* Inline Functions: */
			/** Initializes the serial stream (and regular USART driver) so that both the stream and regular
			 *  USART driver functions can be used. Must be called before any stream or regular USART functions.
			 *
			 *  \param BaudRate  Baud rate to configure the USART at
			 */
			static inline void SerialStream_Init(const uint16_t BaudRate)
			{
				Serial_Init(BaudRate);
				
				stdout = &USARTStream;
			}

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif
