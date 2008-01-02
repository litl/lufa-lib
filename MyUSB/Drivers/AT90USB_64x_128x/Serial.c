/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Serial.h"

void Serial_Init(const uint16_t BaudRate)
{
	UCSR1A = 0;
	UCSR1B = ((1 << RXEN1)  | (1 << TXEN1));
	UCSR1C = ((1 << UCSZ11) | (1 << UCSZ10));
	
	UBRR1  = SERIAL_UBBRVAL(BaudRate);
}

void Serial_TxString_P(const char *FlashStringPtr)
{
	uint8_t CurrByte;

	while ((CurrByte = pgm_read_byte(FlashStringPtr)) != 0x00)
	{
		Serial_TxByte(CurrByte);
		FlashStringPtr++;
	}
}

void Serial_TxString(const char *StringPtr)
{
	uint8_t CurrByte;

	while ((CurrByte = *StringPtr) != 0x00)
	{
		Serial_TxByte(CurrByte);
		StringPtr++;
	}
}
