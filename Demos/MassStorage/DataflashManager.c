/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#define INCLUDE_FROM_DATAFLASHMANAGER_C
#include "DataflashManager.h"

void VirtualMemory_WriteBlocks(const uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint16_t CurrDFPage        = (((uint32_t)BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) / DATAFLASH_PAGE_SIZE);
	uint16_t CurrDFByte        = (((uint32_t)BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) % DATAFLASH_PAGE_SIZE);
	uint8_t  SubBlockTransfers = 0;

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
	
		/* Wait until endpoint ready to be read from again */
		while (!(Endpoint_ReadWriteAllowed()));

		/* Write data to the dataflash buffer in groups of 64 bytes (endpoint size) */
		for (uint8_t WriteLoop = 0; WriteLoop < MASS_STORAGE_IO_EPSIZE; WriteLoop++)
		  Dataflash_SendByte(Endpoint_Read_Byte());
		
		/* Update dataflash buffer counter and sub block counter */
		CurrDFByte += MASS_STORAGE_IO_EPSIZE;
		SubBlockTransfers++;

		/* Acknowedge the endpoint packet, switch to next endpoint bank */
		Endpoint_ClearCurrentBank();

		/* Check if end of block reached */
		if (SubBlockTransfers == VIRTUAL_MEMORY_EPPACKETS_PER_BLOCK)
		{
			/* Decrement the blocks remaining counter and reset the sub block counter */
			TotalBlocks--;
			SubBlockTransfers = 0;
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

void VirtualMemory_ReadBlocks(const uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint16_t CurrDFPage        = (((uint32_t)BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) / DATAFLASH_PAGE_SIZE);
	uint16_t CurrDFByte        = (((uint32_t)BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) % DATAFLASH_PAGE_SIZE);
	uint8_t  SubBlockTransfers = 0;

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

		/* Wait until endpoint ready to be written to again */
		while (!(Endpoint_ReadWriteAllowed()));

		/* Read data from the dataflash in groups of 64 bytes (endpoint size) */
		for (uint8_t ReadLoop = 0; ReadLoop < MASS_STORAGE_IO_EPSIZE; ReadLoop++)
		  Endpoint_Write_Byte(Dataflash_SendByte(0));
		
		/* Send endpoint data */
		Endpoint_ClearCurrentBank();

		/* Update dataflash page byte and sub block counters */
		CurrDFByte += MASS_STORAGE_IO_EPSIZE;
		SubBlockTransfers++;

		/* Check if end of block reached */
		if (SubBlockTransfers == VIRTUAL_MEMORY_EPPACKETS_PER_BLOCK)
		{
			/* Decrement the blocks remaining counter and reset the sub block counter */
			TotalBlocks--;
			SubBlockTransfers = 0;
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
	
	/* Select second dataflash chip (if present on selected board), send read status register command */
	#if (DATAFLASH_TOTALCHIPS == 2)
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
	#endif
	
	/* Deselect current dataflash chip */
	Dataflash_DeselectChip();
}
