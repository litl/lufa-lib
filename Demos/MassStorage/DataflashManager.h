/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _DATAFLASH_MANAGER_H
#define _DATAFLASH_MANAGER_H

	/* Includes: */
		#include <avr/io.h>
		
		#include "MassStorage.h"
		#include "Descriptors.h"

		#include <MyUSB/Common/Common.h>              // Function Attribute, Atomic, Debug and ISR Macros
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/Board/Dataflash.h>    // Dataflash chip driver

	/* Defines: */
		#define VIRTUAL_MEMORY_BYTES                ((uint32_t)(DATAFLASH_PAGES * DATAFLASH_TOTALCHIPS) \
		                                            * DATAFLASH_PAGE_SIZE)
		#define VIRTUAL_MEMORY_BLOCK_SIZE           512
		#define VIRTUAL_MEMORY_BLOCKS               ((VIRTUAL_MEMORY_BYTES / VIRTUAL_MEMORY_BLOCK_SIZE) - 1)
		#define VIRTUAL_MEMORY_EPPACKETS_PER_BLOCK  (VIRTUAL_MEMORY_BLOCK_SIZE / MASS_STORAGE_IO_EPSIZE)
		
	/* Function Prototypes: */
		void VirtualMemory_WriteBlocks(const uint32_t BlockAddress, uint16_t TotalBlocks);
		void VirtualMemory_ReadBlocks(const uint32_t BlockAddress, uint16_t TotalBlocks);
		void VirtualMemory_ResetDataflashProtections(void);

		#if defined(INCLUDE_FROM_DATAFLASHMANAGER_C)
			static uint16_t VirtualMemory_DFPageFromBlock(const uint16_t BlockAddress) ATTR_CONST;
			static uint16_t VirtualMemory_DFPageOffsetFromBlock(const uint16_t BlockAddress) ATTR_CONST;
		#endif
		
#endif
