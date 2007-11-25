/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "CDC_Serial_Stream.h"

FILE CDCStream = FDEV_SETUP_STREAM(CDCStream_TxByte, CDCStream_RxByte, _FDEV_SETUP_RW);

int CDCStream_TxByte(char DataByte, FILE *Stream)
{
	return 0; 
}

int CDCStream_RxByte(FILE *Stream)
{
	return 0;
}
