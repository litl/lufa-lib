/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/* Must be included after avr/interrupt.h. This file re-defines the new
   ISR macro to extend it to allow custom attributes. When the old ISR
   macros SIGNAL and INTERRUPT were depricated, no suitable replacement
   was specifed for interruptable ISR routine (and no macro at all exists
   for naked ISRs). This file avoids the clumsyness of declaring the ISR
   routines manually with custom attributes and thus gives code uniformity.

   As a bonus, the default vector (called when an interrupt fires which does
   not have an associated ISR routine) is aliased here to a more descriptive
   name - use the new name BADISR_vect as you would a standard signal name.
   
   The new macro is backwards compatible with the original ISR macro.
   
   The avaliable attributes are:
      1) ISR_BLOCK         - ISR, interrupts disable until ISR completes.
      2) ISR_NOBLOCK       - ISR, interrupts enabled until ISR completes.
      3) ISR_NAKED         - ISR, no prologue or epilogue.
      4) ISR_ALIASOF(vect) - ISR, alias to another interrupt vector's ISR. GCC 4.2+ only.

   For GCC 3.x vector aliases, you can use the ISR_ALIAS_COMPAT macro (instead
   of ISR macro). Works with GCC 3.x as well as GCC 4.x, but compat aliased vector
   ISR will contain a JMP instruction that the non-compat aliased vector does not have.
*/

#ifndef ISRMACRO_H
#define ISRMACRO_H

	/* Preprocessor Checks and Defines */
		#if defined(ISR)
			#undef ISR
		#endif
   
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define BADISR_vect __vector_default
			#define reti() asm volatile ("RETI"::)

			#if defined(__GNUC__) && (__GNUC__ > 3)
				#define ISR_NOBLOCK    __attribute__((interrupt, used, externally_visible))
				#define ISR_BLOCK      __attribute__((signal, used, externally_visible))
				#define ISR_NAKED      __attribute__((signal, naked, used, externally_visible))
				#define ISR_ALIASOF(v) __attribute__((alias(__replace_and_string(v)))) // GCC 4.2 and greater only!
			#else
				#define ISR_NOBLOCK   __attribute__((interrupt))
				#define ISR_BLOCK     __attribute__((signal))
				#define ISR_NAKED     __attribute__((signal, naked))
			#endif

			#define ISR_ALIAS_COMPAT(vector, aliasof)      \
			   void vector (void) ISR_NAKED;               \
			   void vector (void) { asm volatile ("jmp " __replace_and_string(aliasof) ::); }

			#define ISR(vector, ...)                       \
			   void vector (void) ISR_BLOCK __VA_ARGS__;   \
			   void vector (void)
			   
	/* Private Interface - For use in library only: */
		/* Macros: */
			#define __replace_and_string(name) #name
 
#endif
