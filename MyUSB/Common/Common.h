/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef COMMON_H
#define COMMON_H

	/* Includes */
		#include <stdio.h>

	/* Public Macros */
		#define MACROS                  do
		#define MACROE                  while (0)
		
		#define JTAG_DEBUG_POINT()      asm volatile ("NOP" ::)
		#define JTAG_DEBUG_BREAK()      asm volatile ("BREAK" ::)
		
		#define JTAG_DEBUG_ASSERT(x)    MACROS{ if (!(x)) { JTAG_DEBUG_BREAK(); } }MACROE
		#define SERIAL_STREAM_ASSERT(x) MACROS{ if (!(x)) { printf("%s: Function \"%s\", Line %d: Assertion \"%s\" failed.\r\n", __FILE__, __func__, __LINE__, #x); } }MACROE

#endif
