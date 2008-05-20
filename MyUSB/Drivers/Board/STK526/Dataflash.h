/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Board specific HWB driver header for the STK525.
 *
 *  \note This file should not be included directly. It is automatically included as needed by the dataflash driver
 *        dispatch header located in MyUSB/Drivers/Board/Dataflash.h.
 */

#ifndef __DATAFLASH_STK526_H__
#define __DATAFLASH_STK526_H__

	/* Includes: */
		#include "AT45DB642D.h"

	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_DATAFLASH_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/Dataflash.h instead.
		#endif

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define DATAFLASH_CHIPCS_MASK                (1 << 2)
			#define DATAFLASH_CHIPCS_DDR                 DDRC
			#define DATAFLASH_CHIPCS_PORT                PORTC
	#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Constant indicating the total number of dataflash ICs mounted on the selected board. */
			#define DATAFLASH_TOTALCHIPS                 1

			/** Mask for no dataflash chip selected. */
			#define DATAFLASH_NO_CHIP                    DATAFLASH_CHIPCS_MASK

			/** Mask for the first dataflash chip selected. */
			#define DATAFLASH_CHIP1                      0
			
			/** Internal main memory page size for the board's dataflash IC. */
			#define DATAFLASH_PAGE_SIZE                  1024

			/** Total number of pages inside the board's dataflash IC. */
			#define DATAFLASH_PAGES                      8192

#endif
