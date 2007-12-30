/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __DYN_ALLOC__
#define __DYN_ALLOC__

	/* Includes : */
		#include <avr/io.h>
		#include <stdio.h>
		#include <stdbool.h>
		
		#include "../Common/Common.h"
	
	/* Preprocessor Checks: */
		#if (!defined(NUM_BLOCKS) || !defined(BLOCK_SIZE) || !defined(NUM_HANDLES))
			#error NUM_BLOCKS, BLOCK_SIZE and NUM_HANDLES must be defined before use via makefile.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define DEREF(handle, type)       (*(type*)handle)
		
		/* Type Defines: */
			typedef const void** Mem_Handle_t;

		/* Function Prototypes: */
			Mem_Handle_t Mem_Alloc(const uint16_t Bytes);
			Mem_Handle_t Mem_Calloc(const uint16_t Bytes);
			Mem_Handle_t Mem_Realloc(Mem_Handle_t CurrAllocHdl, const uint16_t Bytes);
			void         Mem_Free(Mem_Handle_t CurrAllocHdl);
			uint8_t      Mem_TotalFreeBlocks(void);
			uint8_t      Mem_TotalFreeHandles(void);
		
	/* Private Interface - For use in library only: */
		/* Macros: */
			#define BLOCK_USED_MASK           (1 << 0)
			#define BLOCK_LINKED_MASK         (1 << 1)
			
		/* Function Prototypes: */
			#if defined(INCLUDE_FROM_DYNALLOC_C)
				static uint8_t Mem_GetBlockFlags(const uint8_t BlockNum);
				static void    Mem_SetBlockFlags(const uint8_t BlockNum, const uint8_t Flags);
				static void    Mem_Defrag(void);
			#endif

#endif
