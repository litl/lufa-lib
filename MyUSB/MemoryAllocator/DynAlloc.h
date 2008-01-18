/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __DYN_ALLOC__
#define __DYN_ALLOC__

	/* Includes : */
		#include <avr/io.h>
		#include <stdbool.h>
		#include <string.h>
	
	/* Preprocessor Checks: */
		#if (!defined(NUM_BLOCKS) || !defined(BLOCK_SIZE) || !defined(NUM_HANDLES))
			#error NUM_BLOCKS, BLOCK_SIZE and NUM_HANDLES must be defined before use via makefile.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define DEREF(handle, type)       (*(type*)handle)
			#define ALLOCABLE_BYTES           (NUM_BLOCKS * BLOCK_SIZE)
		
		/* Type Defines: */
			typedef const void** Mem_Handle_t;
			
			#if (NUM_BLOCKS > 0xFFFF)
				typedef uint32_t Block_Number_t;
			#elif (NUM_BLOCKS > 0xFF)
				typedef uint16_t Block_Number_t;
			#else			
				typedef uint8_t  Block_Number_t;
			#endif

			#if (NUM_HANDLES > 0xFFFF)
				typedef uint32_t Handle_Number_t;
			#elif (NUM_HANDLES > 0xFF)
				typedef uint16_t Handle_Number_t;
			#else			
				typedef uint8_t  Handle_Number_t;
			#endif
			
			#if (ALLOCABLE_BYTES > 0xFFFF)
				typedef uint32_t Alloc_Size_t;
			#elif (ALLOCABLE_BYTES > 0xFF)
				typedef uint16_t Alloc_Size_t;
			#else			
				typedef uint8_t  Alloc_Size_t;
			#endif			
			
		/* Function Prototypes: */
			Mem_Handle_t    Mem_Alloc(const Alloc_Size_t Bytes);
			Mem_Handle_t    Mem_Calloc(const Alloc_Size_t Bytes);
			Mem_Handle_t    Mem_Realloc(Mem_Handle_t CurrAllocHdl, const Alloc_Size_t Bytes);
			void            Mem_Free(Mem_Handle_t CurrAllocHdl);
			Block_Number_t  Mem_TotalFreeBlocks(void);
			Handle_Number_t Mem_TotalFreeHandles(void);
		
	/* Private Interface - For use in library only: */
		/* Macros: */
			#define BLOCK_USED_MASK           (1 << 0)
			#define BLOCK_LINKED_MASK         (1 << 1)
			
		/* Function Prototypes: */
			#if defined(INCLUDE_FROM_DYNALLOC_C)
				static uint8_t Mem_GetBlockFlags(const Block_Number_t BlockNum);
				static void    Mem_SetBlockFlags(const Block_Number_t BlockNum, const uint8_t Flags);
				static void    Mem_Defrag(void);
			#endif

#endif
