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

#ifndef __DATAFLASH_USBKEY_H__
#define __DATAFLASH_USBKEY_H__

	/* Includes: */
		#include "AT45DB642D.h"

	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_DATAFLASH_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/Dataflash.h instead.
		#endif
		
	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define DATAFLASH_CHIPCS_MASK                ((1 << 1) | (1 << 0))
			#define DATAFLASH_CHIPCS_DDR                 DDRE
			#define DATAFLASH_CHIPCS_PORT                PORTE
	#endif
	
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Constant indicating the total number of dataflash ICs mounted on the selected board. */
			#define DATAFLASH_TOTALCHIPS                 2

			/** Mask for no dataflash chip selected. */
			#define DATAFLASH_NO_CHIP                    DATAFLASH_CHIPCS_MASK

			/** Mask for the first dataflash chip selected. */
			#define DATAFLASH_CHIP1                      (1 << 1)

			/** Mask for the second dataflash chip selected. */
			#define DATAFLASH_CHIP2                      (1 << 0)
			
			/** Internal main memory page size for the board's dataflash ICs. */
			#define DATAFLASH_PAGE_SIZE                  1024

			/** Total number of pages inside each of the board's dataflash ICs. */
			#define DATAFLASH_PAGES                      8192

#endif
