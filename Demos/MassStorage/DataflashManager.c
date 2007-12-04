/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "DataflashManager.h"

void VirtualMemory_WriteBlocks(uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint16_t CurrDFPage       = VirtualMemory_DFPageFromBlock(BlockAddress);
	uint16_t CurrDFByte       = VirtualMemory_DFPageOffsetFromBlock(BlockAddress);
	uint16_t BytesInCurrBlock = 0;

	Dataflash_SelectChipFromPage(CurrDFPage);
	
	Dataflash_SendByte(DF_CMD_MAINMEMTOBUFF1);
	VirtualMemory_SendAddressBytes(CurrDFPage, 0);

	Dataflash_ToggleSelectedChipCS();
		
	Dataflash_SendByte(DF_CMD_GETSTATUS);
	while (!(Dataflash_SendByte(0x00) & DF_STATUS_READY));
	
	Dataflash_ToggleSelectedChipCS();

	Dataflash_SendByte(DF_CMD_BUFF1WRITE);
	VirtualMemory_SendAddressBytes(0, CurrDFByte);

	while (TotalBlocks)
	{
		if (CurrDFByte == DATAFLASH_PAGE_SIZE)
		{
			Dataflash_ToggleSelectedChipCS();

			Dataflash_SendByte(DF_CMD_BUFF1TOMAINMEMWITHERASE);
			VirtualMemory_SendAddressBytes(CurrDFPage, 0);
			
			Dataflash_ToggleSelectedChipCS();

			Dataflash_SendByte(DF_CMD_GETSTATUS);	
			while (!(Dataflash_SendByte(0x00) & DF_STATUS_READY));

			CurrDFByte = 0;
			CurrDFPage++;

			Dataflash_SelectChip(DATAFLASH_NO_CHIP);
			Dataflash_SelectChipFromPage(CurrDFPage);

			Dataflash_SendByte(DF_CMD_MAINMEMTOBUFF1);
			VirtualMemory_SendAddressBytes(CurrDFPage, 0);

			Dataflash_ToggleSelectedChipCS();

			Dataflash_SendByte(DF_CMD_GETSTATUS);	
			while (!(Dataflash_SendByte(0x00) & DF_STATUS_READY));

			Dataflash_ToggleSelectedChipCS();

			Dataflash_SendByte(DF_CMD_BUFF1WRITE);
			VirtualMemory_SendAddressBytes(0, 0);
		}

		while (!(Endpoint_Out_IsRecieved()));
		
		Dataflash_SendByte(Endpoint_Read_Byte());
		
		CurrDFByte++;
		BytesInCurrBlock++;

		if (!(Endpoint_BytesInEndpoint()))
		  Endpoint_Out_Clear();
	
		if (BytesInCurrBlock == VIRTUAL_MEMORY_BLOCK_SIZE)
		{
			TotalBlocks--;
			BytesInCurrBlock = 0;
		}
	}

	Dataflash_ToggleSelectedChipCS();
			
	Dataflash_SendByte(DF_CMD_BUFF1TOMAINMEMWITHERASE);
	VirtualMemory_SendAddressBytes(CurrDFPage, 0x00);
			
	Dataflash_ToggleSelectedChipCS();

	Dataflash_SendByte(DF_CMD_GETSTATUS);	
	while (!(Dataflash_SendByte(0x00) & DF_STATUS_READY));

	Dataflash_SelectChip(DATAFLASH_NO_CHIP);
}

void VirtualMemory_ReadBlocks(uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint16_t CurrDFPage       = VirtualMemory_DFPageFromBlock(BlockAddress);
	uint16_t CurrDFByte       = VirtualMemory_DFPageOffsetFromBlock(BlockAddress);
	uint16_t BytesInCurrBlock = 0;
	uint8_t  BytesInEndpoint  = 0;

	Dataflash_SelectChipFromPage(CurrDFPage);
	
	Dataflash_SendByte(DF_CMD_MAINMEMPAGEREAD);
	VirtualMemory_SendAddressBytes(CurrDFPage, CurrDFByte);

	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);

	while (TotalBlocks)
	{
		if (CurrDFByte == DATAFLASH_PAGE_SIZE)
		{
			CurrDFByte = 0;
			CurrDFPage++;

			Dataflash_SelectChip(DATAFLASH_NO_CHIP);
			Dataflash_SelectChipFromPage(CurrDFPage);

			Dataflash_SendByte(DF_CMD_MAINMEMPAGEREAD);
			VirtualMemory_SendAddressBytes(CurrDFPage, 0);

			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
		}

		if (BytesInEndpoint == MASS_STORAGE_IO_EPSIZE)
		{
			Endpoint_In_Clear();
			while (!(Endpoint_ReadWriteAllowed()));			
			
			BytesInEndpoint = 0;
		}

		Endpoint_Write_Byte(Dataflash_SendByte(0));
		
		CurrDFByte++;
		BytesInCurrBlock++;
		BytesInEndpoint++;

		if (BytesInCurrBlock == VIRTUAL_MEMORY_BLOCK_SIZE)
		{
			TotalBlocks--;
			BytesInCurrBlock = 0;
		}
	}

	Dataflash_SelectChip(DATAFLASH_NO_CHIP);

	if (BytesInEndpoint)
	  Endpoint_In_Clear();
}

void VirtualMemory_SendAddressBytes(uint16_t PageAddress, const uint16_t BufferAddress)
{
	if (Dataflash_GetSelectedChip() == DATAFLASH_CHIP2)
	  PageAddress -= DATAFLASH_PAGES;

	Dataflash_SendAddressBytes(PageAddress, BufferAddress);
}

uint16_t VirtualMemory_DFPageFromBlock(const uint32_t BlockAddress)
{
	return ((BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) / DATAFLASH_PAGE_SIZE);
}

uint16_t VirtualMemory_DFPageOffsetFromBlock(const uint32_t BlockAddress)
{
	return ((BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) % DATAFLASH_PAGE_SIZE);
}
