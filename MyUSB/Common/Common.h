/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __COMMON_H__
#define __COMMON_H__

	/* Includes: */
		#include <stdio.h>
		#include <avr/version.h>
		
		#include "FunctionAttributes.h"
		
		#if (__AVR_LIBC_VERSION__ < 10501UL)		
			#include "ISRMacro.h"
			#include "Atomic.h"			
		#else
			#include <util/atomic.h>
			#include <avr/interrupt.h>
			#include <alloca.h>
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#if !defined(alloca)
				#define alloca(x)           __builtin_alloca(x)
			#endif
		
			#define MACROS                  do
			#define MACROE                  while (0)
			
			#define JTAG_DEBUG_POINT()      asm volatile ("NOP" ::)
			#define JTAG_DEBUG_BREAK()      asm volatile ("BREAK" ::)
			
			#define JTAG_DEBUG_ASSERT(x)    MACROS{ if (!(x)) { JTAG_DEBUG_BREAK(); } }MACROE
			#define SERIAL_STREAM_ASSERT(x) MACROS{ if (!(x)) { printf_P(PSTR("%s: Function \"%s\", Line %d: Assertion \"%s\" failed.\r\n"), __FILE__, __func__, __LINE__, #x); } }MACROE

		/* Inline Functions: */
			static inline void SetSystemClockPrescaler(uint8_t x)
			{
					uint8_t tmp = _BV(CLKPCE);
					__asm__ __volatile__ (
							"in __tmp_reg__,__SREG__" "\n\t"
							"cli" "\n\t"
							"sts %1, %0" "\n\t"
							"sts %1, %2" "\n\t"
							"out __SREG__, __tmp_reg__"
							: /* no outputs */
							: "d" (tmp),
							  "M" (_SFR_MEM_ADDR(CLKPR)),
							  "d" (x)
							: "r0");
			}

#endif
