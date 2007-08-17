/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "Serial_Stream.h"

static FILE USARTStream = FDEV_SETUP_STREAM(Serial_TransmitByte, Serial_RecieveByte, _FDEV_SETUP_RW);

void Serial_Init(const uint16_t BaudRate)
{
	UCSR1A = 0;
	UCSR1B = ((1 << RXEN1) | (1 << TXEN1));
	UCSR1C = ((1 << UCSZ11) | (1 << UCSZ11));
	
	UBRR1  = SERIAL_UBBRVAL(BaudRate);

	stdout = &USARTStream;
}

int Serial_TransmitByte(char DataByte, FILE *Stream)
{
	while (!(UCSR1A & (1 << UDRE1)));
	UDR1 = DataByte;

	return 0; 
}

int Serial_RecieveByte(FILE *Stream)
{
	while (!(UCSR1A & (1 << RXC1)));
	return UDR1; 
}
