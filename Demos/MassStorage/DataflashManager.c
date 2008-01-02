/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#define INCLUDE_FROM_DATAFLASHMANAGER_C
#include "DataflashManager.h"

void VirtualMemory_WriteBlocks(uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint16_t CurrDFPage       = VirtualMemory_DFPageFromBlock(BlockAddress);
	uint16_t CurrDFByte       = VirtualMemory_DFPageOffsetFromBlock(BlockAddress);
	uint16_t BytesInCurrBlock = 0;

	/* Select the dataflash IC based on the page number */
	Dataflash_SelectChipFromPage(CurrDFPage);
	
	/* Copy selected dataflash's current page contents to the dataflash buffer */
	Dataflash_SendByte(DF_CMD_MAINMEMTOBUFF1);
	Dataflash_SendAddressBytes(CurrDFPage, 0);
	Dataflash_WaitWhileBusy();	

	/* Send the dataflash buffer write command */
	Dataflash_ToggleSelectedChipCS();
	Dataflash_SendByte(DF_CMD_BUFF1WRITE);
	Dataflash_SendAddressBytes(0, CurrDFByte);

	/* Wait until the endpoint is ready to be read from */
	while (!(Endpoint_ReadWriteAllowed()));

	while (TotalBlocks)
	{
		/* Check if end of dataflash page reached */
		if (CurrDFByte == DATAFLASH_PAGE_SIZE)
		{
			/* Write the dataflash buffer contents back to the dataflash page */
			Dataflash_ToggleSelectedChipCS();
			Dataflash_SendByte(DF_CMD_BUFF1TOMAINMEMWITHERASE);
			Dataflash_SendAddressBytes(CurrDFPage, 0);

			/* Reset the dataflash buffer counter, increment the page counter */
			CurrDFByte = 0;
			CurrDFPage++;

			/* Select the next dataflash chip based on the new dataflash page index */
			Dataflash_SelectChipFromPage(CurrDFPage);
			
			/* Wait until the selected dataflash is ready to be written to */
			Dataflash_WaitWhileBusy();

			/* Copy selected dataflash's current page contents to the dataflash buffer */
			Dataflash_SendByte(DF_CMD_MAINMEMTOBUFF1);
			Dataflash_SendAddressBytes(CurrDFPage, 0);
			Dataflash_WaitWhileBusy();

			/* Send the dataflash buffer write command */
			Dataflash_ToggleSelectedChipCS();
			Dataflash_SendByte(DF_CMD_BUFF1WRITE);
			Dataflash_SendAddressBytes(0, 0);
		}
	
		/* Write data to the dataflash buffer in groups of 8 bytes */
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		Dataflash_SendByte(Endpoint_Read_Byte());
		
		/* Update both the dataflash buffer counter and the block byte counter */
		CurrDFByte       += 8;
		BytesInCurrBlock += 8;

		/* Check if end of block reached */
		if (BytesInCurrBlock == VIRTUAL_MEMORY_BLOCK_SIZE)
		{
			/* Decrement the blocks remaining counter and reset the block byte counter */
			TotalBlocks--;
			BytesInCurrBlock = 0;
		}

		/* Check if endpoint empty (all data read from it and written to dataflash */
		if (!(Endpoint_BytesInEndpoint()))
		{
			/* Acknowedge the endpoint packet, switch to next endpoint bank */
			Endpoint_FIFOCON_Clear();

			/* Check if any blocks remaining, if so wait until endpoint ready to be read from again */
			if (TotalBlocks)
			  while (!(Endpoint_ReadWriteAllowed()));
		}
	}

	/* Write the dataflash buffer contents back to the dataflash page */
	Dataflash_ToggleSelectedChipCS();
	Dataflash_SendByte(DF_CMD_BUFF1TOMAINMEMWITHERASE);
	Dataflash_SendAddressBytes(CurrDFPage, 0x00);
	Dataflash_WaitWhileBusy();

	/* Deselect all dataflash chips */
	Dataflash_DeselectChip();
}

void VirtualMemory_ReadBlocks(uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint16_t CurrDFPage       = VirtualMemory_DFPageFromBlock(BlockAddress);
	uint16_t CurrDFByte       = VirtualMemory_DFPageOffsetFromBlock(BlockAddress);
	uint16_t BytesInCurrBlock = 0;
	uint8_t  BytesInEndpoint  = 0;

	/* Select the dataflash IC based on the page number */
	Dataflash_SelectChipFromPage(CurrDFPage);
	
	/* Send the dataflash page read command */
	Dataflash_SendByte(DF_CMD_MAINMEMPAGEREAD);
	Dataflash_SendAddressBytes(CurrDFPage, CurrDFByte);

	/* Initialize the internal dataflash buffers - four dummy bytes when using SPI interface */
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	
	/* Wait until endpoint is ready to be written to */
	while (!(Endpoint_ReadWriteAllowed()));

	while (TotalBlocks)
	{
		/* Check if end of dataflash page reached */		
		if (CurrDFByte == DATAFLASH_PAGE_SIZE)
		{
			/* Reset the dataflash page byte counter, increment page index counter */
			CurrDFByte = 0;
			CurrDFPage++;

			/* Select the next dataflash chip based on the new dataflash page index */
			Dataflash_SelectChipFromPage(CurrDFPage);

			/* Send the dataflash page read command */
			Dataflash_SendByte(DF_CMD_MAINMEMPAGEREAD);
			Dataflash_SendAddressBytes(CurrDFPage, 0);

			/* Initialize the internal dataflash buffers - four dummy bytes when using SPI interface */
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
		}

		/* Read data from the dataflash in groups of 8 bytes */
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		
		/* Update dataflash page byte, block byte and endpoint byte counters */
		CurrDFByte       += 8;
		BytesInCurrBlock += 8;
		BytesInEndpoint  += 8;

		/* Check if end of block reached */
		if (BytesInCurrBlock == VIRTUAL_MEMORY_BLOCK_SIZE)
		{
			/* Decrement the blocks remaining counter and reset the block byte counter */
			TotalBlocks--;
			BytesInCurrBlock = 0;
		}

		/* Check if endpoint full */
		if (BytesInEndpoint == MASS_STORAGE_IO_EPSIZE)
		{
			/* Send endpoint data and reset the endpoint byte counter */
			Endpoint_FIFOCON_Clear();
			BytesInEndpoint = 0;
			
			/* Check if any blocks remaining, if so wait until endpoint ready to be written to again */
			if (TotalBlocks)
			  while (!(Endpoint_ReadWriteAllowed()));
		}
	}

	/* Deselect all dataflash chips */
	Dataflash_DeselectChip();
}

void VirtualMemory_ResetDataflashProtections(void)
{
	/* Select first dataflash chip, send the read status register command */
	Dataflash_SelectChip(DATAFLASH_CHIP1);
	Dataflash_SendByte(DF_CMD_GETSTATUS);
	
	/* Check if sector protection is enabled */
	if (Dataflash_SendByte(0x00) & DF_STATUS_SECTORPROTECTION_ON)
	{
		Dataflash_ToggleSelectedChipCS();

		/* Send the commands to disable sector protection */
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE1);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE2);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE3);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE4);
	}
	
	/* Select second dataflash chip, send the read status register command */
	Dataflash_SelectChip(DATAFLASH_CHIP2);
	Dataflash_SendByte(DF_CMD_GETSTATUS);
	
	/* Check if sector protection is enabled */
	if (Dataflash_SendByte(0x00) & DF_STATUS_SECTORPROTECTION_ON)
	{
		Dataflash_ToggleSelectedChipCS();

		/* Send the commands to disable sector protection */
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE1);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE2);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE3);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF_BYTE4);
	}
	
	/* Deselect second dataflash chip */
	Dataflash_DeselectChip();
}

static uint16_t VirtualMemory_DFPageFromBlock(const uint16_t BlockAddress)
{
	/* Translate from logical block address to dataflash page address */
	return (((uint32_t)BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) / DATAFLASH_PAGE_SIZE);
}

static uint16_t VirtualMemory_DFPageOffsetFromBlock(const uint16_t BlockAddress)
{
	/* Translate from logical block address to dataflash buffer address */
	return (((uint32_t)BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) % DATAFLASH_PAGE_SIZE);
}
