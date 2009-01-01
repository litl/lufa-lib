/*
             LUFA Library
     Copyright (C) Dean Camera, 2009.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2009  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Functions to manage the physical dataflash media, including reading and writing of
 *  blocks of data. These functions are called by the SCSI layer when data must be stored
 *  or retrieved to/from the physical storage media. If a different media is used (such
 *  as a SD card or EEPROM), functions similar to these will need to be generated.
 */

#define  INCLUDE_FROM_DATAFLASHMANAGER_C
#include "DataflashManager.h"

/** Writes blocks (OS blocks, not Dataflash pages) to the storage medium, the board dataflash IC(s), from
 *  the pre-selected data OUT endpoint. This routine reads in OS sized blocks from the endpoint and writes
 *  them to the dataflash in Dataflash page sized blocks.
 *
 *  \param BlockAddress  Data block starting address for the write sequence
 *  \param TotalBlocks   Number of blocks of data to write
 */
void VirtualMemory_WriteBlocks(const uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint16_t CurrDFPage = ((BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) / DATAFLASH_PAGE_SIZE);
	uint16_t CurrDFByte = ((BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) % DATAFLASH_PAGE_SIZE);

	/* Create a buffer to hold the incomming endpoint packet's data */
	uint8_t PacketBuffer[VIRTUAL_MEMORY_BLOCK_SIZE];
		
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

	while (TotalBlocks && !(IsMassStoreReset))
	{
		/* Read in the packet data from the host, use the stream function to take advantage of
		   the ready-wait, timeout and early abort code rather than duplicating it here */
		Endpoint_Read_Stream_LE(&PacketBuffer, VIRTUAL_MEMORY_BLOCK_SIZE, AbortOnMassStoreReset);

		/* Acknowedge the endpoint packet, switch to next endpoint bank */
		Endpoint_ClearCurrentBank();

		/* Pointer to the block buffer for fast access */
		uint8_t* BufferPos = (uint8_t*)&PacketBuffer;
		
		/* Counter for the number of bytes processed in the current block */
		uint16_t BufferByteDiv16 = 0;

		/* Write one block of data to the dataflash */
		while (BufferByteDiv16 < (VIRTUAL_MEMORY_BLOCK_SIZE >> 4))
		{
			/* Determine how many 16 byte blocks remain to be read from the current dataflash page and data block */
			uint8_t BytesRemInBlockDiv16 = ((VIRTUAL_MEMORY_BLOCK_SIZE >> 4) - BufferByteDiv16);
			uint8_t BytesRemInPageDiv16  = ((DATAFLASH_PAGE_SIZE - CurrDFByte) >> 4);
			
			/* Determine which which is smaller - process the smaller amount to ensure that we don't either
			   exceed the dataflash page or number of remaining bytes in the block */
			uint8_t BytesToReadDiv16     = (BytesRemInBlockDiv16 < BytesRemInPageDiv16) ? BytesRemInBlockDiv16 :
			                                                                              BytesRemInPageDiv16;

			/* Data is processed 16 bytes at a time for speed - cavet, dataflash page must be a multiple of 16 */
			while (BytesToReadDiv16--)
			{
				/* Write one 16-byte chunk of data to the dataflash */
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				Dataflash_SendByte(*(BufferPos++));
				
				/* Increment the dataflash page byte counter by the number of bytes processed */
				CurrDFByte += 16;

				/* Increment the number of 16 byte chunks processed counter */
				BufferByteDiv16++;
			}

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
		}

		/* Decrement the blocks remaining counter and reset the sub block counter */
		TotalBlocks--;
	}

	/* Write the dataflash buffer contents back to the dataflash page */
	Dataflash_ToggleSelectedChipCS();
	Dataflash_SendByte(DF_CMD_BUFF1TOMAINMEMWITHERASE);
	Dataflash_SendAddressBytes(CurrDFPage, 0x00);
	Dataflash_WaitWhileBusy();

	/* Deselect all dataflash chips */
	Dataflash_DeselectChip();
}

/** Reads blocks (OS blocks, not Dataflash pages) from the storage medium, the board dataflash IC(s), into
 *  the pre-selected data IN endpoint. This routine reads in Dataflash page sized blocks from the Dataflash
 *  and writes them in OS sized blocks to the endpoint.
 *
 *  \param BlockAddress  Data block starting address for the read sequence
 *  \param TotalBlocks   Number of blocks of data to read
 */
void VirtualMemory_ReadBlocks(const uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint16_t CurrDFPage = ((BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) / DATAFLASH_PAGE_SIZE);
	uint16_t CurrDFByte = ((BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE) % DATAFLASH_PAGE_SIZE);

	/* Create a buffer to hold the outgoing endpoint packet's data */
	uint8_t PacketBuffer[VIRTUAL_MEMORY_BLOCK_SIZE];
		
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

	while (TotalBlocks && !(IsMassStoreReset))
	{
		/* Pointer to the block buffer for fast access */
		uint8_t* BufferPos = (uint8_t*)&PacketBuffer;
		
		/* Counter for the number of bytes processed in the current block */
		uint16_t BufferByteDiv16 = 0;
		
		/* Read in a block of data from the dataflash */
		while (BufferByteDiv16 < (VIRTUAL_MEMORY_BLOCK_SIZE >> 4))
		{
			/* Determine how many 16 byte blocks remain to be read from the current dataflash page and data block */
			uint8_t BytesRemInBlockDiv16 = ((VIRTUAL_MEMORY_BLOCK_SIZE >> 4) - BufferByteDiv16);
			uint8_t BytesRemInPageDiv16  = ((DATAFLASH_PAGE_SIZE - CurrDFByte) >> 4);
			
			/* Determine which which is smaller - process the smaller amount to ensure that we don't either
			 * exceed the dataflash page or number of remaining bytes in the block */
			uint8_t BytesToReadDiv16     = (BytesRemInBlockDiv16 < BytesRemInPageDiv16) ? BytesRemInBlockDiv16 :
			                                                                              BytesRemInPageDiv16;
		
			/* Data is processed 16 bytes at a time for speed - cavet, dataflash page must be a multiple of 16 */
			while (BytesToReadDiv16--)
			{
				/* Read one 16-byte chunk of data from the dataflash */
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				*(BufferPos++) = Dataflash_SendByte(0);
				
				/* Increment the dataflash page byte counter by the number of bytes processed */
				CurrDFByte += 16;

				/* Increment the number of 16 byte chunks processed counter */
				BufferByteDiv16++;
			}

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
		}

		/* Write out the packet data to the host, use the stream function to take advantage of
		   the ready-wait, timeout and early abort code rather than duplicating it here */
		Endpoint_Write_Stream_LE(&PacketBuffer, VIRTUAL_MEMORY_BLOCK_SIZE, AbortOnMassStoreReset);
		
		/* Send endpoint data */
		Endpoint_ClearCurrentBank();

		/* Decrement the blocks remaining counter */
		TotalBlocks--;
	}

	/* Deselect all dataflash chips */
	Dataflash_DeselectChip();
}

/** Disables the dataflash memory write protection bits on the board Dataflash ICs, if enabled. */
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
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF[0]);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF[1]);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF[2]);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF[3]);
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
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF[0]);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF[1]);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF[2]);
		Dataflash_SendByte(DF_CMD_SECTORPROTECTIONOFF[3]);
	}
	#endif
	
	/* Deselect current dataflash chip */
	Dataflash_DeselectChip();
}
