/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _BOOTFUNCS_H_
#define _BOOTFUNCS_H_

	/* Includes: */
		#include <avr/io.h>

	/* Macros: */
		#define  BOOT_SIZE_KB             4

		#define  TOTAL_FLASH_PAGES        (((FLASHEND + 1) - (BOOT_SIZE_KB * 1024)) \
		                                   / SPM_PAGESIZE)

		#define  MEM_ADDR(sfr)           _SFR_MEM_ADDR(sfr)
		#define  IO_ADDR(sfr)            _SFR_IO_ADDR(sfr)

		#define  HIGH(val)               (val >> 8)
		#define  LOW(val)                (val & 0xFF)

		#define  ZL                      r30
		#define  ZH                      r31

	/* Function Prototypes: */
		#if !defined(__ASSEMBLER__)
			uint8_t boot_read_sig_byte(uint8_t Address);
			void    wtd_disable_at90usb1287(void);
		#endif

#endif
