/*
             LUFA Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _DATAFLASH_MANAGER_H
#define _DATAFLASH_MANAGER_H

	/* Includes: */
		#include <avr/io.h>
		
		#include "MassStorage.h"
		#include "Descriptors.h"

		#include <LUFA/Common/Common.h>              // Function Attribute, Atomic, Debug and ISR Macros
		#include <LUFA/Drivers/USB/USB.h>            // USB Functionality
		#include <LUFA/Drivers/Board/Dataflash.h>    // Dataflash chip driver

	/* Defines: */
		#define VIRTUAL_MEMORY_BYTES                ((uint32_t)(DATAFLASH_PAGES * DATAFLASH_TOTALCHIPS) \
		                                            * DATAFLASH_PAGE_SIZE)
		#define VIRTUAL_MEMORY_BLOCK_SIZE           512
		#define VIRTUAL_MEMORY_BLOCKS               ((VIRTUAL_MEMORY_BYTES / VIRTUAL_MEMORY_BLOCK_SIZE) - 1)
		
	/* Function Prototypes: */
		void VirtualMemory_WriteBlocks(const uint32_t BlockAddress, uint16_t TotalBlocks);
		void VirtualMemory_ReadBlocks(const uint32_t BlockAddress, uint16_t TotalBlocks);
		void VirtualMemory_ResetDataflashProtections(void);
		
#endif
