/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "Dataflash.h"

void Dataflash_Init(void)
{
	PINB  |= (1 << 0);
	DDRB  |= ((1 << 1) | (1 << 2));
	DDRE  |= DATAFLASH_CHIPCS_MASK;
	PORTE |= DATAFLASH_CHIPCS_MASK;

	SPCR  = ((1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA));
}

bool Dataflash_SelectChipFromPage(const uint16_t PageAddress)
{
	if (PageAddress < DATAFLASH_PAGES)
	{
		Dataflash_SelectChip(DATAFLASH_CHIP1);
		return true;
	}
	else if (PageAddress < (DATAFLASH_PAGES * 2))
	{
		Dataflash_SelectChip(DATAFLASH_CHIP2);
		return true;
	}
	else
	{
		Dataflash_SelectChip(DATAFLASH_NO_CHIP);
		return false;
	}
}

void Dataflash_SendAddressBytes(const uint16_t PageAddress, const uint16_t BufferByte)
{
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
