/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "ADC.h"

uint16_t ADC_GetChannelReading(uint8_t Channel)
{
	uint16_t ConvertedResult;

	ADCSRA = ((1 << ADEN) | (7 << ADPS0));	
	ADMUX = Channel;
	
	ADCSRA |= (1 << ADSC);
	
	ConvertedResult = ADC;
	
	ADCSRA = 0;
	
	return ConvertedResult;
}
