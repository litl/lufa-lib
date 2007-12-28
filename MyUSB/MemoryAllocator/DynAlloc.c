/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#define INCLUDE_FROM_DYNALLOC_C
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

static inline void Mem_Defrag(void)
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

static inline bool Mem_FindFreeBlocks_PRV(uint8_t* const RetStartPtr, const uint8_t Blocks, const bool ExactMatch)
{
	uint8_t FreeInCurrSec = 0;

	for (int16_t CurrBlock = (NUM_BLOCKS - 1); CurrBlock >= -1; CurrBlock--)
	{
		if (!(Mem_GetBlockFlags(CurrBlock) & BLOCK_USED_MASK) && (CurrBlock != -1))
		{
			FreeInCurrSec++;
		}
		else
		{
			if ((FreeInCurrSec >= Blocks) && (!(ExactMatch && (FreeInCurrSec != Blocks))))
			{
				*RetStartPtr = (CurrBlock + 1);
				return true;
			}
			
			FreeInCurrSec = 0;
		}
	}

	return false;
}

void** Mem_Alloc(const uint16_t Bytes)
{
	uint8_t ReqBlocks = (Bytes / BLOCK_SIZE);
	uint8_t StartBlock;
	uint8_t Blocks;
	bool    Allocate = false;
	
	if (Bytes % BLOCK_SIZE)
	  ReqBlocks++;
	
	for (uint8_t SearchBlockSize = ReqBlocks; SearchBlockSize < NUM_BLOCKS; SearchBlockSize++)
	{
		if (Mem_FindFreeBlocks_PRV(&StartBlock, SearchBlockSize, true))
		{
			Blocks   = SearchBlockSize;
			Allocate = true;
			break;
		}
	}
	
	if (!(Allocate))
	{
		Mem_Defrag();
		
		if (Mem_FindFreeBlocks_PRV(&StartBlock, ReqBlocks, false))
		{
			Blocks   = ReqBlocks;
			Allocate = true;
		}
	}

	if (Allocate)
	{
		for (uint8_t UsedBlock = 0; UsedBlock < (ReqBlocks - 1); UsedBlock++)
		  Mem_SetBlockFlags((StartBlock + UsedBlock), BLOCK_USED_MASK | BLOCK_LINKED_MASK);

		Mem_SetBlockFlags((StartBlock + (ReqBlocks - 1)), BLOCK_USED_MASK);
		
		for (uint8_t AllocEntry = 0; AllocEntry < ALLOC_TABLE_SIZE; AllocEntry++)
		{
			if (Mem_MemData.Mem_AllocTable[AllocEntry] == NULL)
			{
				Mem_MemData.Mem_AllocTable[AllocEntry] = &Mem_MemData.Mem_Heap[StartBlock * BLOCK_SIZE];
				return &Mem_MemData.Mem_AllocTable[AllocEntry];
			}
		}		
	}

	return NULL;
}

void** Mem_Realloc_PRV(const void** CurrAllocPtr, const uint16_t Bytes)
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

void Mem_Free_PRV(const void** MemPtr)
{
	const void* MemBlockPtr = *MemPtr;
	uint8_t CurrBlock = ((uint16_t)((char*)MemBlockPtr - Mem_MemData.Mem_Heap) / BLOCK_SIZE);
	uint8_t CurrBlockFlags;

	if ((MemPtr == NULL) || (MemBlockPtr == NULL))
	  return;

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
