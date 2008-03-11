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
		#include "AT45DB642D.h"

	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_DATAFLASH_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/Dataflash.h instead.
		#endif
		
	/* Private Interface - For use in library only: */
		/* Macros: */
			#define DATAFLASH_CHIPCS_MASK                ((1 << 1) | (1 << 0))
			#define DATAFLASH_CHIPCS_DDR                 DDRE
			#define DATAFLASH_CHIPCS_PORT                PORTE

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define DATAFLASH_TOTALCHIPS                 2

			#define DATAFLASH_NO_CHIP                    DATAFLASH_CHIPCS_MASK
			#define DATAFLASH_CHIP1                      (1 << 1)
			#define DATAFLASH_CHIP2                      (1 << 0)
			
			#define DATAFLASH_PAGE_SIZE                  1024
			#define DATAFLASH_PAGES                      8192

#endif
