/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#define INCLUDE_FROM_DATAFLASHMANAGER_C
#include "DataflashManager.h"

void VirtualMemory_WriteBlocks(uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint16_t CurrDFPage       = VirtualMemory_DFPageFromBlock(BlockAddress);
	uint16_t CurrDFByte       = VirtualMemory_DFPageOffsetFromBlock(BlockAddress);
	uint16_t BytesInCurrBlock = 0;

	Dataflash_SelectChipFromPage(CurrDFPage);
	
	Dataflash_SendByte(DF_CMD_MAINMEMTOBUFF1);
	Dataflash_SendAddressBytes(CurrDFPage, 0);
	Dataflash_WaitWhileBusy();	

	Dataflash_ToggleSelectedChipCS();
	Dataflash_SendByte(DF_CMD_BUFF1WRITE);
	Dataflash_SendAddressBytes(0, CurrDFByte);

	while (!(Endpoint_ReadWriteAllowed()));

	while (TotalBlocks)
	{
		if (CurrDFByte == DATAFLASH_PAGE_SIZE)
		{
			Dataflash_ToggleSelectedChipCS();
			Dataflash_SendByte(DF_CMD_BUFF1TOMAINMEMWITHERASE);
			Dataflash_SendAddressBytes(CurrDFPage, 0);
			Dataflash_WaitWhileBusy();

			CurrDFByte = 0;
			CurrDFPage++;

			Dataflash_SelectChip(DATAFLASH_NO_CHIP);
			Dataflash_SelectChipFromPage(CurrDFPage);

			Dataflash_SendByte(DF_CMD_MAINMEMTOBUFF1);
			Dataflash_SendAddressBytes(CurrDFPage, 0);
			Dataflash_WaitWhileBusy();

			Dataflash_ToggleSelectedChipCS();
			Dataflash_SendByte(DF_CMD_BUFF1WRITE);
			Dataflash_SendAddressBytes(0, 0);
		}

		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		
		CurrDFByte       += 8;
		BytesInCurrBlock += 8;

		if (BytesInCurrBlock == VIRTUAL_MEMORY_BLOCK_SIZE)
		{
			TotalBlocks--;
			BytesInCurrBlock = 0;
		}

		if (!(Endpoint_BytesInEndpoint()))
		{
			Endpoint_Out_Clear();

			if (TotalBlocks)
			  while (!(Endpoint_Out_IsRecieved()));
		}
	}

	Dataflash_ToggleSelectedChipCS();
	Dataflash_SendByte(DF_CMD_BUFF1TOMAINMEMWITHERASE);
	Dataflash_SendAddressBytes(CurrDFPage, 0x00);
	Dataflash_WaitWhileBusy();

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
	Dataflash_SendAddressBytes(CurrDFPage, CurrDFByte);

	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	
	while (!(Endpoint_ReadWriteAllowed()));

	while (TotalBlocks)
	{
		if (CurrDFByte == DATAFLASH_PAGE_SIZE)
		{
			CurrDFByte = 0;
			CurrDFPage++;

			Dataflash_SelectChip(DATAFLASH_NO_CHIP);
			Dataflash_SelectChipFromPage(CurrDFPage);

			Dataflash_SendByte(DF_CMD_MAINMEMPAGEREAD);
			Dataflash_SendAddressBytes(CurrDFPage, 0);

			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
		}

		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		
		CurrDFByte       += 8;
		BytesInCurrBlock += 8;
		BytesInEndpoint  += 8;

		if (BytesInCurrBlock == VIRTUAL_MEMORY_BLOCK_SIZE)
		{
			TotalBlocks--;
			BytesInCurrBlock = 0;
		}

		if (BytesInEndpoint == MASS_STORAGE_IO_EPSIZE)
		{
			Endpoint_In_Clear();
			BytesInEndpoint = 0;
			
			if (TotalBlocks)
			  while (!(Endpoint_ReadWriteAllowed()));
		}
	}

	Dataflash_SelectChip(DATAFLASH_NO_CHIP);
}

void VirtualMemory_ResetDataflashProtections(void)
{
	Dataflash_SelectChip(DATAFLASH_CHIP1);
	Dataflash_SendByte(DF_CMD_GETSTATUS);
	
	if (Dataflash_SendByte(0x00) & DF_STATUS_SECTORPROTECTION_ON)
	{
		Dataflash_ToggleSelectedChipCS();

		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE1);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE2);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE3);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE4);
	}
	
	Dataflash_SelectChip(DATAFLASH_CHIP2);
	Dataflash_SendByte(DF_CMD_GETSTATUS);
	
	if (Dataflash_SendByte(0x00) & DF_STATUS_SECTORPROTECTION_ON)
	{
		Dataflash_ToggleSelectedChipCS();

		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE1);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE2);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE3);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE4);
	}
	
	Dataflash_SelectChip(DATAFLASH_NO_CHIP);
}

static uint16_t VirtualMemory_DFPageFromBlock(const uint16_t BlockAddress)
{
	return (((uint32_t)BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) / DATAFLASH_PAGE_SIZE);
}

static uint16_t VirtualMemory_DFPageOffsetFromBlock(const uint16_t BlockAddress)
{
	return (((uint32_t)BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) % DATAFLASH_PAGE_SIZE);
}
