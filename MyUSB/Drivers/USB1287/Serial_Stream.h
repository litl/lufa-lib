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

    /* Private Macros */
		#define SERIAL_UBBRVAL(baud)   (((F_CPU / (baud * 16UL))) - 1)
	
	/* Function Prototypes */
		void Serial_Init(const uint16_t BaudRate);
		
	/* Private Functions */
		int  Serial_TransmitByte(char DataByte, FILE *Stream);
		int  Serial_RecieveByte(FILE *Stream);
#endif
