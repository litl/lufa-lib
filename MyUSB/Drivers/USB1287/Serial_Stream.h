/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __SERIAL_STREAM_H__
#define __SERIAL_STREAM_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdio.h>
		
		#include "Serial.h"
	
	/* Private Interface - For use in library only: */		
		/* External Variables: */
			extern FILE USARTStream;

		/* Function Prototypes: */
			int SerialStream_TxByte(char DataByte, FILE *Stream) ATTR_NON_NULL_PTR_ARG(2);
			int SerialStream_RxByte(FILE *Stream) ATTR_NON_NULL_PTR_ARG(1);

	/* Public Interface - May be used in end-application: */
		/* Inline Functions: */
			static inline void SerialStream_Init(const uint16_t BaudRate)
			{
				Serial_Init(BaudRate);
				
				stdout = &USARTStream;
			}

#endif
