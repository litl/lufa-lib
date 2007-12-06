/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "Dataflash.h"

void Dataflash_SelectChipFromPage(const uint16_t PageAddress)
{
	if (PageAddress < DATAFLASH_PAGES)
	  Dataflash_SelectChip(DATAFLASH_CHIP1);
	else if (PageAddress < (DATAFLASH_PAGES * 2))
	  Dataflash_SelectChip(DATAFLASH_CHIP2);
	else
	  Dataflash_SelectChip(DATAFLASH_NO_CHIP);
}

void Dataflash_SendAddressBytes(uint16_t PageAddress, const uint16_t BufferByte)
{
	if (Dataflash_GetSelectedChip() == DATAFLASH_CHIP2)
	  PageAddress -= DATAFLASH_PAGES;

	Dataflash_SendByte(PageAddress >> 5);
	Dataflash_SendByte((PageAddress << 3) | (BufferByte >> 8));
	Dataflash_SendByte(BufferByte);
}

void Dataflash_ToggleSelectedChipCS(void)
{
	uint8_t SelectedChipMask = Dataflash_GetSelectedChip();
	
	Dataflash_SelectChip(DATAFLASH_NO_CHIP);
	Dataflash_SelectChip(SelectedChipMask);
}
