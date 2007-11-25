/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __CDC_SERIAL_STREAM_H__
#define __CDC_SERIAL_STREAM_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdio.h>
		
		#include <MyUSB/Common/FunctionAttributes.h>
			
	/* External Variables: */
		extern FILE CDCStream;

	/* Function Prototypes: */
		int CDCStream_TxByte(char DataByte, FILE *Stream) ATTR_NON_NULL_PTR_ARG(2);
		int CDCStream_RxByte(FILE *Stream) ATTR_NON_NULL_PTR_ARG(1);

#endif
