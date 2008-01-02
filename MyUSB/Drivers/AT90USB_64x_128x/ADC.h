/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __ADC_H__
#define __ADC_H__

	/* Includes: */
		#include <avr/io.h>

		#include "../../Common/Common.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define  ADC_Init(mode)          MACROS{ ADCSRA = ((1 << ADEN) | mode);         }MACROE
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
			
			#define  ADC_FREE_RUNNING                (1 << ADATE)
			#define  ADC_SINGLE_CONVERSION           (0 << ADATE)
			
			#define  ADC_PRESCALE_2                  (1 << ADPS0)
			#define  ADC_PRESCALE_4                  (1 << ADPS1)
			#define  ADC_PRESCALE_8                  ((1 << ADPS0) | (1 << ADPS1))
			#define  ADC_PRESCALE_16                 (1 << ADPS2)
			#define  ADC_PRESCALE_32                 ((1 << ADPS2) | (1 << ADPS0))
			#define  ADC_PRESCALE_64                 ((1 << ADPS2) | (1 << ADPS1))
			#define  ADC_PRESCALE_128                ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))

		/* Inline Functions: */
			static inline void ADC_SetupChannel(const uint8_t Channel)
			{
				DDRD  &= ~(1 << Channel);
				DIDR0 |=  (1 << Channel);
			}
			
			static inline void ADC_StartReading(const uint8_t MUXMask)
			{
				ADMUX = MUXMask;
			
				ADCSRA |= (1 << ADSC);
			}

			static inline uint16_t ADC_GetChannelReading(const uint8_t MUXMask) ATTR_WARN_UNUSED_RESULT;
			static inline uint16_t ADC_GetChannelReading(const uint8_t MUXMask)
			{
				ADC_StartReading(MUXMask);
	
				while (!(ADC_IsReadingComplete()));
	
				return ADC_GetResult();
			}

#endif
