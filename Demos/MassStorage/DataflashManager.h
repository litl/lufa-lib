/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef _DATAFLASH_MANAGER_H
#define _DATAFLASH_MANAGER_H

	// Includes:
		#include <avr/io.h>
		
		#include "MassStorage.h"
		#include "Descriptors.h"

		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/USBKEY/Dataflash.h>   // Dataflash chip driver for the USBKEY

	// Defines:
		#define VIRTUAL_MEMORY_BLOCK_SIZE           512
		
		#define VIRTUAL_MEMORY_BLOCKS               (((uint32_t)(DATAFLASH_PAGES * 2) *      \
		                                               DATAFLASH_PAGE_SIZE) /                \
		                                               VIRTUAL_MEMORY_BLOCK_SIZE)
													
	// Function Prototypes:
		void     VirtualMemory_WriteBlocks(uint32_t BlockAddress, uint16_t TotalBlocks);
		void     VirtualMemory_ReadBlocks(uint32_t BlockAddress, uint16_t TotalBlocks);
		uint16_t VirtualMemory_DFPageFromBlock(const uint32_t BlockAddress);
		uint16_t VirtualMemory_DFPageOffsetFromBlock(const uint32_t BlockAddress);
		void     VirtualMemory_SendAddressBytes(uint16_t PageAddress, const uint16_t BufferAddress);
		
#endif
