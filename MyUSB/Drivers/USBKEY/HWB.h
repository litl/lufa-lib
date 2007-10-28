/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef HWB_H
#define HWB_H

	/* Includes */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../Common/FunctionAttributes.h"
		
	/* Public Interface - May be used in end-application: */
		/* Inline Functions */
			static inline void HWB_Init(void)
			{
				DDRE  &= ~(1 << 2);
				PORTE |= (1 << 2);
			}

			static inline bool HWB_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline bool HWB_GetStatus(void)
			{
				return (!(PINE & (1 << 2)));
			}
			
#endif
