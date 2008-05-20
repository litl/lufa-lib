/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  This file contains macros which are common to all library elements, and which may be useful in user code. It
 *  also includes other common headees, such as Atomic.h, FunctionAttributes.h and BoardTypes.h.
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
		
			/** Macro for encasing other multi-statment macros. This should be used along with an opening brace
			 *  before the start of any multi-statement macro, so that the macros contents as a whole are treated
			 *  as a discreete block and not as a list of seperate statements which may cause problems when used as
			 *  a block (such as inline IF statments).
			 */
			#define MACROS                  do

			/** Macro for encasing other multi-statment macros. This should be used along with a preceeding closing
			 *  brace at the end of any multi-statement macro, so that the macros contents as a whole are treated
			 *  as a discreete block and not as a list of seperate statements which may cause problems when used as
			 *  a block (such as inline IF statments).
			 */
			#define MACROE                  while (0)
			
			/** Defines a volatile NOP statment which cannot be optimized out by the compiler, and thus can always
			 *  be set as a breakpoint in the resulting code. Useful for debugging purposes, where the optimizer
			 *  removes/reorders code to the point where break points cannot reliably be set.
			 */
			#define JTAG_DEBUG_POINT()      asm volatile ("NOP" ::)

			/** Defines an explicit JTAG break point in the resulting binary via the ASM BREAK statment. When
			 *  a JTAG is used, this causes the program execution to halt when reached until manually resumed. */
			#define JTAG_DEBUG_BREAK()      asm volatile ("BREAK" ::)
			
			/** Macro for testing condition "x" and breaking via JTAG_DEBUG_BREAK() if the condition is false. */
			#define JTAG_DEBUG_ASSERT(x)    MACROS{ if (!(x)) { JTAG_DEBUG_BREAK(); } }MACROE

			/** Macro for testing condition "x" and writing debug data to the serial stream if false. As a
			 *  prerequisite for this macro, the serial stream should be configured via the Serial_Stream driver.
			 *
			 *  The serial output takes the form "{FILENAME}: Function {FUNCTION NAME}, Line {LINE NUMBER}: Assertion
			 *  {x} failed."
			 */
			#define SERIAL_STREAM_ASSERT(x) MACROS{ if (!(x)) { printf_P(PSTR("%s: Function \"%s\", Line %d: "   \
																"Assertion \"%s\" failed.\r\n"),   \
																__FILE__, __func__, __LINE__, #x); \
			                                } }MACROE

		/* Inline Functions: */
			/** Function for reliably setting the AVR's system clock prescaler, using inline assembly. This function
			 *  is guaranteed to operate reliably regardless of optimization setting or other compile time options. 
			 *
			 * \param PrescalerMask   The mask of the new prescaler setting for CLKPR
			 */
			static inline void SetSystemClockPrescaler(uint8_t PrescalerMask)
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
							  "d" (PrescalerMask)
							: "r0");
			}

#endif
