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
	PINB |= (1 << 0);
	DDRE |= DATAFLASH_CHIPCS_MASK;

	SPCR  = ((1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0));
	SPSR  = (1 << SPI2X);	
		
	Dataflash_SelectChip(DATAFLASH_NO_CHIP);
}

void Dataflash_WaitWhileBusy(void)
{
	Dataflash_SendByte(DF_CMD_GETSTATUS);
	
	while (!(Dataflash_SendByte(0x00) & DF_STATUS_READY));
}

uint8_t Dataflash_SendByte(const uint8_t Byte)
{
	SPDR = Byte;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}

bool Dataflash_SelectChipFromPage(uint16_t PageAddress)
{
	if (PageAddress < DATAFLASH_PAGE_SIZE)
	{
		Dataflash_SelectChip(DATAFLASH_CHIP1);
		return true;
	}
	else if (PageAddress < (DATAFLASH_PAGE_SIZE * 2))
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

void Dataflash_ToggleSelectedChipCS(void)
{
	uint8_t SelectedChipMask = Dataflash_GetSelectedChip();
	
	Dataflash_SelectChip(DATAFLASH_NO_CHIP);
	Dataflash_SelectChip(SelectedChipMask);
}
