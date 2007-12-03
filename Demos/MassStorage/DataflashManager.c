/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "DataflashManager.h"

void VirtualMemory_WriteBlocks(uint32_t* BlockAddress, uint16_t* TotalBlocks)
{
	// TODO
}

void VirtualMemory_ReadBlocks(uint32_t* BlockAddress, uint16_t* TotalBlocks)
{
	// TODO
}
	
/*    ** READ **
	Dataflash_SelectChipFromPage(BlockAddress);

	Dataflash_SendByte(DF_CMD_MAINMEMPAGEREAD);
	Dataflash_SendByte(BlockAddress >> 8);
	Dataflash_SendByte((BlockAddress & 0xFF) << 5);
	Dataflash_SendByte(0);

	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);

	while (BlocksRem)
	{
		if (AddressInBlock == DATAFLASH_PAGE_SIZE)
		{
			Dataflash_SelectChip(DATAFLASH_NO_CHIP);			
			Dataflash_SelectChipFromPage(BlockAddress);

			Dataflash_SendByte(DF_CMD_MAINMEMPAGEREAD);
			Dataflash_SendByte(BlockAddress >> 8);
			Dataflash_SendByte((BlockAddress & 0xFF) << 5);
			Dataflash_SendByte(0);

			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);			

			AddressInBlock = 0;
			BlocksRem--;
			BlockAddress++;
		}
		
		Endpoint_Write_Byte(Dataflash_SendByte(0));
		AddressInBlock++;
	}
*/	
	
/*	  ** WRITE **
	Dataflash_SelectChipFromPage(BlockAddress);
	
	Dataflash_SendByte(DF_CMD_BUFF1WRITE);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);
	Dataflash_SendByte(0);

	while (BlocksRem)
	{
		if (AddressInBlock == DATAFLASH_PAGE_SIZE)
		{
			Dataflash_ToggleSelectedChipCS();
			
			Dataflash_SendByte(DF_CMD_BUFF1TOMAINMEMWITHERASE);
			Dataflash_SendByte(BlockAddress >> 8);
			Dataflash_SendByte((BlockAddress & 0xFF) << 5);
			Dataflash_SendByte(0);

			Dataflash_ToggleSelectedChipCS();
			Dataflash_SendByte(DF_CMD_GETSTATUS);
			
			while (!(Dataflash_SendByte(0) & DF_STATUS_READY));

			Dataflash_SelectChip(DATAFLASH_NO_CHIP);			
			
			AddressInBlock = 0;
			BlocksRem--;
			BlockAddress++;
			
			Dataflash_SelectChipFromPage(BlockAddress);

			Dataflash_SendByte(DF_CMD_BUFF1WRITE);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
			Dataflash_SendByte(0);
		}

		Dataflash_SendByte(Endpoint_Read_Byte());

		AddressInBlock++;
	}

	Dataflash_SelectChip(DATAFLASH_NO_CHIP);
*/
