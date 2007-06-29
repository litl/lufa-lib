#ifndef HWB_H
#define HWB_H

	/* Includes */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../Common/FunctionAttributes.h"
		
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
