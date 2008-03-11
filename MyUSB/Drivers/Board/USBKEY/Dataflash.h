/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __DATAFLASH_USBKEY_H__
#define __DATAFLASH_USBKEY_H__

	/* Includes: */
		#include <avr/io.h>
		
		#include "../../../Common/Common.h"
		#include "../../Misc/DataflashCommandBytes.h"

	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_DATAFLASH_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/Dataflash.h instead.
		#endif
		
	/* Private Interface - For use in library only: */
		/* Macros: */
			#define DATAFLASH_CHIPCS_MASK                (DATAFLASH_CHIP1 | DATAFLASH_CHIP2)
			#define DATAFLASH_USE_DOUBLESPEED            (1 << 7)

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define DATAFLASH_TOTALCHIPS                 2

			#define DATAFLASH_NO_CHIP                    DATAFLASH_CHIPCS_MASK
			#define DATAFLASH_CHIP1                      (1 << 1)
			#define DATAFLASH_CHIP2                      (1 << 0)
			
			#define DATAFLASH_PAGE_SIZE                  1024
			#define DATAFLASH_PAGES                      8192
			
			#define DATAFLASH_SPEED_FCPU_DIV_2           DATAFLASH_USE_DOUBLESPEED
			#define DATAFLASH_SPEED_FCPU_DIV_4           0
			#define DATAFLASH_SPEED_FCPU_DIV_8           (DATAFLASH_USE_DOUBLESPEED | (1 << SPR0))
			#define DATAFLASH_SPEED_FCPU_DIV_16          (1 << SPR0)
			#define DATAFLASH_SPEED_FCPU_DIV_32          (DATAFLASH_USE_DOUBLESPEED | (1 << SPR1))
			#define DATAFLASH_SPEED_FCPU_DIV_64          (DATAFLASH_USE_DOUBLESPEED | (1 << SPR1) | (1 < SPR0))
			#define DATAFLASH_SPEED_FCPU_DIV_128         ((1 << SPR1) | (1 < SPR0))
					
			#define Dataflash_GetSelectedChip()          (PORTE & DATAFLASH_CHIPCS_MASK)
			#define Dataflash_SelectChip(mask)   MACROS{ PORTE = ((PORTE & ~DATAFLASH_CHIPCS_MASK) | mask); }MACROE
			#define Dataflash_DeselectChip()             Dataflash_SelectChip(DATAFLASH_NO_CHIP)

		/* Inline Functions: */
			static inline void Dataflash_Init(const uint8_t PrescalerMask)
			{
				DDRB  |= ((1 << 1) | (1 << 2));
				PORTB |= (1 << 0);

				DDRE  |= DATAFLASH_CHIPCS_MASK;
				PORTE |= DATAFLASH_CHIPCS_MASK;

				SPCR   = ((1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) |
				          (PrescalerMask & ~DATAFLASH_USE_DOUBLESPEED));

				if (PrescalerMask & DATAFLASH_USE_DOUBLESPEED)
				  SPSR = (1 << SPI2X);
			}

#endif
