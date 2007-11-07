/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef SERIAL_STREAM_H
#define SERIAL_STREAM_H

	/* Includes: */
		#include <avr/io.h>
		#include <stdio.h>
		
		#include "Serial.h"
	
	/* Private Interface - For use in library only: */		
		/* External Variables: */
			extern FILE USARTStream;

		/* Function Prototypes: */
			int SerialStream_TxByte(char DataByte, FILE *Stream);
			int SerialStream_RxByte(FILE *Stream);

	/* Public Interface - May be used in end-application: */
		/* Inline Functions: */
			static inline void SerialStream_Init(const uint16_t BaudRate)
			{
				Serial_Init(BaudRate);
				
				stdout = &USARTStream;
			}

#endif
