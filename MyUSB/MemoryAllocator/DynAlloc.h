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
		#if (!defined(NUM_BLOCKS) || !defined(BLOCK_SIZE) || !defined(ALLOC_TABLE_SIZE))
			#error NUM_BLOCKS, BLOCK_SIZE and ALLOC_TABLE_SIZE must be defined before use via makefile.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define Mem_Realloc(ptr, bytes)   Mem_Realloc_PRV((const void**)ptr, bytes)  
			#define Mem_Free(ptr)             Mem_Free_PRV((const void**)ptr)
		
		/* Function Prototypes: */
			void**  Mem_Alloc(const uint16_t Bytes);
			void**  Mem_Calloc(const uint16_t Bytes);
			uint8_t Mem_TotalFreeBlocks(void);
		
	/* Private Interface - For use in library only: */
		/* Macros: */
			#define BLOCK_USED_MASK           (1 << 0)
			#define BLOCK_LINKED_MASK         (1 << 1)
			
		/* Function Prototypes: */
			void**  Mem_Realloc_PRV(const void** CurrAllocPtr, const uint16_t Bytes);
			void    Mem_Free_PRV(const void** MemPtr);

			#if defined(INCLUDE_FROM_DYNALLOC_C)
				static uint8_t Mem_GetBlockFlags(const uint8_t BlockNum);
				static void    Mem_SetBlockFlags(const uint8_t BlockNum, const uint8_t Flags);
				static void    Mem_Defrag(void);
			#endif

#endif
