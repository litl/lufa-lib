/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "DynAlloc.h"

struct
{
	char    Mem_Heap[NUM_BLOCKS * BLOCK_SIZE];
	void*   Mem_AllocTable[ALLOC_TABLE_SIZE];
	uint8_t Mem_Block_Flags[(NUM_BLOCKS / 4) + 1];
	uint8_t FlagMaskLookupMask[4];
	uint8_t FlagMaskLookupNum[4];
} Mem_MemData = {FlagMaskLookupMask:  {(3 << 0), (3 << 2), (3 << 4), (3 << 6)},
                 FlagMaskLookupNum:   {      0,        2,        4,        6}};

static uint8_t Mem_GetBlockFlags(const uint8_t BlockNum)
{
	const uint8_t BlockIndex    = (BlockNum >> 2);
	const uint8_t FlagMask      = Mem_MemData.FlagMaskLookupMask[BlockNum & 0x03];
	const uint8_t FlagMaskShift = Mem_MemData.FlagMaskLookupNum[BlockNum & 0x03];

	return ((Mem_MemData.Mem_Block_Flags[BlockIndex] & FlagMask) >> FlagMaskShift);
}

static void Mem_SetBlockFlags(const uint8_t BlockNum, const uint8_t Flags)
{
	const uint8_t BlockIndex    = (BlockNum >> 2);
	const uint8_t FlagMask      = Mem_MemData.FlagMaskLookupMask[BlockNum & 0x03];
	const uint8_t FlagMaskShift = Mem_MemData.FlagMaskLookupNum[BlockNum & 0x03];

	Mem_MemData.Mem_Block_Flags[BlockIndex] &= ~FlagMask;
	Mem_MemData.Mem_Block_Flags[BlockIndex] |= (Flags << FlagMaskShift);
}

static void Mem_Defrag(void)
{
	char*   FreeStart   = NULL;
	uint8_t StartOfFree = 0;
	
	for (uint8_t CurrBlock = 0; CurrBlock < NUM_BLOCKS; CurrBlock++)
	{
		if (!(Mem_GetBlockFlags(CurrBlock) & BLOCK_USED_MASK))
		{
			FreeStart      = &Mem_MemData.Mem_Heap[CurrBlock * BLOCK_SIZE];
			StartOfFree    = CurrBlock;
			break;
		}
	}
	
	if (FreeStart == NULL)
	  return;

	for (uint8_t CurrBlock = StartOfFree; CurrBlock < NUM_BLOCKS; CurrBlock++)
	{
		if (Mem_GetBlockFlags(CurrBlock) & BLOCK_USED_MASK)
		{
			char* UsedStart = &Mem_MemData.Mem_Heap[CurrBlock * BLOCK_SIZE];
			
			for (uint8_t AllocEntry = 0; AllocEntry < ALLOC_TABLE_SIZE; AllocEntry++)
			{
				if (Mem_MemData.Mem_AllocTable[AllocEntry] == UsedStart)
				{
					Mem_MemData.Mem_AllocTable[AllocEntry] = FreeStart;
					break;
				}
			}

			for (uint8_t BlockByte = 0; BlockByte < BLOCK_SIZE; BlockByte++)
			  *(FreeStart++) = *(UsedStart++);
			  
			Mem_SetBlockFlags(StartOfFree, Mem_GetBlockFlags(CurrBlock));
			Mem_SetBlockFlags(CurrBlock, 0);
			
			StartOfFree++;
		}
	}
}

void** Mem_Realloc_PRV(void** CurrAllocPtr, const uint16_t Bytes)
{
	Mem_Free(CurrAllocPtr);
	return Mem_Alloc(Bytes);
}

void** Mem_Calloc(const uint16_t Bytes)
{
	void** AllocPtr = Mem_Alloc(Bytes);
	
	if (AllocPtr != NULL)
	{
		char* ClearPtr = *AllocPtr;
	
		for (uint16_t ClearPos = 0; ClearPos < Bytes; ClearPos++)
		  *(ClearPtr++) = 0x00;	
	}

	return AllocPtr;
}

void** Mem_Alloc(const uint16_t Bytes)
{
	uint8_t ReqBlocks = (Bytes / BLOCK_SIZE);
	bool    Defragged = false;
	
	if (Bytes % BLOCK_SIZE)
	  ReqBlocks++;
	
AllocStart:
	
	for (uint8_t SearchBlockSize = ReqBlocks; SearchBlockSize < NUM_BLOCKS; SearchBlockSize++)
	{
		uint8_t FreeInCurrSec  = 0;
		uint8_t StartOfFree    = 0;
		void*   StartOfFreePtr = NULL;
		bool    FoundStart     = false;

		for (uint8_t CurrBlock = 0; CurrBlock < NUM_BLOCKS; CurrBlock++)
		{
			if (!(FoundStart) && !(Mem_GetBlockFlags(CurrBlock) & BLOCK_USED_MASK))
			{
				StartOfFreePtr = &Mem_MemData.Mem_Heap[CurrBlock * BLOCK_SIZE];
				StartOfFree    = CurrBlock;
				FreeInCurrSec  = 0;
				FoundStart     = true;
			}
			
			if (FoundStart)
			{
				if (!(Mem_GetBlockFlags(CurrBlock) & BLOCK_USED_MASK))
				  FreeInCurrSec++;
				else
				  FoundStart = false;
				  
				if (FreeInCurrSec == SearchBlockSize)
				{
					for (uint8_t UsedBlock = 0; UsedBlock < SearchBlockSize; UsedBlock++)
					  Mem_SetBlockFlags((StartOfFree + UsedBlock), BLOCK_USED_MASK | BLOCK_LINKED_MASK);

					Mem_SetBlockFlags(((StartOfFree + SearchBlockSize) - 1), BLOCK_USED_MASK);
					
					for (uint8_t AllocEntry = 0; AllocEntry < ALLOC_TABLE_SIZE; AllocEntry++)
					{
						if (Mem_MemData.Mem_AllocTable[AllocEntry] == NULL)
						{
							Mem_MemData.Mem_AllocTable[AllocEntry] = StartOfFreePtr;
							return &Mem_MemData.Mem_AllocTable[AllocEntry];
						}
					}
				}
			}
		}
	}
	
	if (!(Defragged))
	{
		Defragged = true;
		Mem_Defrag();
		goto AllocStart;
	}

	return NULL;
}

void Mem_Free_PRV(void** MemPtr)
{
	if ((MemPtr == NULL) || (*MemPtr == NULL))
	  return;

	uint8_t CurrBlock = ((uint16_t)((char*)*MemPtr - Mem_MemData.Mem_Heap) / BLOCK_SIZE);
	uint8_t CurrBlockFlags;

	do
	{
		CurrBlockFlags = Mem_GetBlockFlags(CurrBlock);
		Mem_SetBlockFlags(CurrBlock, 0);

		CurrBlock++;
	}
	while (CurrBlockFlags & BLOCK_LINKED_MASK);
	
	*MemPtr = NULL;
}

uint8_t Mem_TotalFreeBlocks(void)
{
	uint8_t FreeBlocks = 0;
	
	for (uint8_t CurrBlock = 0; CurrBlock < NUM_BLOCKS; CurrBlock++)
	{
		if (!(Mem_GetBlockFlags(CurrBlock) & BLOCK_USED_MASK))
		  FreeBlocks++;
	}
	
	return FreeBlocks;
}
