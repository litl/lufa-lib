/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef ATOMIC_H
#define ATOMIC_H

	/* Includes: */
		#include <avr/io.h>
		#include <avr/interrupt.h>

	/* Public Interface - May be used in end-application: */
		/* Macros */
			#define ATOMIC_BLOCK(exitmode)     for ( exitmode, __ToDo = __iCliRetVal(); __ToDo ; __ToDo = 0 )

			#define ATOMIC_RESTORESTATE        uint8_t sreg_save __attribute__((__cleanup__(__iRestore)))  = SREG
			#define ATOMIC_FORCEON             uint8_t sreg_save __attribute__((__cleanup__(__iSeiParam))) = 0
			
			#define NON_ATOMIC_BLOCK(exitmode) for ( exitmode, __ToDo = __iSeiRetVal(); __ToDo ; __ToDo = 0 )
			
			#define NONATOMIC_RESTORESTATE     uint8_t sreg_save __attribute__((__cleanup__(__iRestore)))  = SREG
			#define NONATOMIC_FORCEOFF         uint8_t sreg_save __attribute__((__cleanup__(__iCliParam))) = 0

	/* Private Interface - For use in library only: */
		/* Inline Functions */
		static __inline__ uint8_t __iSeiRetVal(void)               { sei(); return 1; }   
		static __inline__ uint8_t __iCliRetVal(void)               { cli(); return 1; }   
		static __inline__ void    __iSeiParam(const uint8_t* __s)  { sei(); __asm__ volatile ("" ::: "memory"); (void)__s; }
		static __inline__ void    __iCliParam(const uint8_t* __s)  { cli(); __asm__ volatile ("" ::: "memory"); (void)__s; }
		static __inline__ void    __iRestore(const  uint8_t* __s)  { SREG = *__s; __asm__ volatile ("" ::: "memory"); }
		
#endif
