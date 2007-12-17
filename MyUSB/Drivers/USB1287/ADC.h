/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __ADC_H__
#define __ADC_H__

	/* Includes: */
		#include <avr/io.h>

		#include "../../Common/FunctionAttributes.h"
		#include "../../Common/Common.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define  ADC_Init()              MACROS{ ADCSRA = ((1 << ADEN) | (7 << ADPS0)); }MACROE
			#define  ADC_Off()               MACROS{ ADCSRA = 0;                            }MACROE
			#define  ADC_On()                MACROS{ ADCSRA = ((1 << ADEN) | (7 << ADPS0)); }MACROE
			#define  ADC_GetStatus()                 (ADCSRA & (1 << ADEN)

			#define  ADC_IsReadingComplete()         (!(ADCSRA & (1 << ADSC)))
			#define  ADC_GetResult()                 ADC
			
			#define  ADC_REFERENCE_AREF              0
			#define  ADC_REFERENCE_AVCC              (1 << REFS0)
			#define  ADC_REFERENCE_INT2560MV         ((1 << REFS1)| (1 << REFS0))
			
			#define  ADC_LEFT_ADJUSTED               (1 << ADLAR)
			#define  ADC_RIGHT_ADJUSTED              (0 << ADLAR)

		/* Inline Functions: */
			static inline void ADC_SetupChannel(const uint8_t Channel)
			{
				DDRD  &= ~(1 << Channel);
				DIDR0 |=  (1 << Channel);
			}
			
			static inline void ADC_StartReading(const uint8_t RefAndChannel)
			{
				ADMUX = RefAndChannel;
			
				ADCSRA |= (1 << ADSC);
			}

			static inline uint16_t ADC_GetChannelReading(const uint8_t RefAndChannel) ATTR_WARN_UNUSED_RESULT;
			static inline uint16_t ADC_GetChannelReading(const uint8_t RefAndChannel)
			{
				ADC_StartReading(RefAndChannel);
	
				while (!(ADC_IsReadingComplete()));
	
				return ADC_GetResult();
			}

#endif
