/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef ADC_H
#define ADC_H

	/* Includes */
		#include <avr/io.h>

		#include "../../Common/FunctionAttributes.h"

	/* Public Macros */
		#define  ADC_Init()              ADCSRA = ((1 << ADEN) | (7 << ADPS0))
		#define  ADC_Off()               ADCSRA = 0
		#define  ADC_On()                ADCSRA = ((1 << ADEN) | (7 << ADPS0))
		#define  ADC_GetStatus()         (ADCSRA & (1 << ADEN)

		#define  ADC_IsReadingComplete() (!(ADCSRA & (1 << ADSC)))
		#define  ADC_GetReading()        ADC

	/* Inline Functions */
	static inline void ADC_SetupChannel(const uint8_t Channel)
	{
		DDRD  &= ~(1 << Channel);
		DIDR0 |= (1 << Channel);
	}
	
	static inline void ADC_StartReading(const uint8_t Channel)
	{
		ADMUX = Channel;
	
		ADCSRA |= (1 << ADSC);
	}
	
	static inline uint16_t ADC_ReadChannel(const uint8_t Channel) ATTR_WARN_UNUSED_RESULT;
	static inline uint16_t ADC_ReadChannel(const uint8_t Channel)
	{
		ADC_StartReading(Channel);

		while (!(ADC_IsReadingComplete()));

		return ADC_GetReading();
	}

#endif
