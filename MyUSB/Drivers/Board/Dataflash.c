/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Dataflash.h"

void Dataflash_SelectChipFromPage(const uint16_t PageAddress)
{
	if (PageAddress > (DATAFLASH_PAGES * DATAFLASH_TOTALCHIPS))
	{
		Dataflash_SelectChip(DATAFLASH_NO_CHIP);
		return;
	}

	#if (DATAFLASH_TOTALCHIPS == 2)
		if (PageAddress & 0x01)
		  Dataflash_SelectChip(DATAFLASH_CHIP2);
		else
		  Dataflash_SelectChip(DATAFLASH_CHIP1);
	#else
		Dataflash_SelectChip(DATAFLASH_NO_CHIP);
		Dataflash_SelectChip(DATAFLASH_CHIP1);
	#endif
}

void Dataflash_SendAddressBytes(uint16_t PageAddress, const uint16_t BufferByte)
{	
	#if (DATAFLASH_TOTALCHIPS == 2)
		PageAddress >>= 1;
	#endif

	Dataflash_SendByte(PageAddress >> 5);
	Dataflash_SendByte((PageAddress << 3) | (BufferByte >> 8));
	Dataflash_SendByte(BufferByte);
}

void Dataflash_ToggleSelectedChipCS(void)
{
	#if (DATAFLASH_TOTALCHIPS == 2)
		uint8_t SelectedChipMask = Dataflash_GetSelectedChip();
		
		Dataflash_SelectChip(DATAFLASH_NO_CHIP);
		Dataflash_SelectChip(SelectedChipMask);
	#else
		Dataflash_SelectChip(DATAFLASH_NO_CHIP);
		Dataflash_SelectChip(DATAFLASH_CHIP1);	
	#endif
}
