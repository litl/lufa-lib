/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef SERIAL_STREAM_H
#define SERIAL_STREAM_H

	/* Includes */
		#include <avr/io.h>
		#include <stdio.h>

	/* Public Interface - May be used in end-application: */
		/* Function Prototypes */
			void Serial_Init(const uint16_t BaudRate);

	/* Private Interface - For use in library only: */
		/* Macros */
			#define SERIAL_UBBRVAL(baud)   (((F_CPU / (baud * 16UL))) - 1)
		
		/* Function Prototypes */
			int Serial_TxByte(char DataByte, FILE *Stream);
			int Serial_RxByte(FILE *Stream);

#endif
