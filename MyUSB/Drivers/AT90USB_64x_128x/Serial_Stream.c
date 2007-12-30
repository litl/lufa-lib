/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Serial_Stream.h"

FILE USARTStream = FDEV_SETUP_STREAM(SerialStream_TxByte, SerialStream_RxByte, _FDEV_SETUP_RW);

int SerialStream_TxByte(char DataByte, FILE *Stream)
{
	Serial_TxByte(DataByte);

	return 0; 
}

int SerialStream_RxByte(FILE *Stream)
{
	return Serial_RxByte();
}
