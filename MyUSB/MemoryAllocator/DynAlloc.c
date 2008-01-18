/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#define INCLUDE_FROM_DYNALLOC_C
#include "DynAlloc.h"

struct
{
	char    Mem_Heap[NUM_BLOCKS * BLOCK_SIZE];
	void*   Mem_Handles[NUM_HANDLES];
	uint8_t Mem_Block_Flags[(NUM_BLOCKS / 4) + ((NUM_BLOCKS % 4) ? 1 : 0)];
	uint8_t FlagMaskLookupMask[4];
	uint8_t FlagMaskLookupNum[4];
} Mem_MemData = {FlagMaskLookupMask:  {(3 << 0), (3 << 2), (3 << 4), (3 << 6)},
                 FlagMaskLookupNum:   {      0,        2,        4,        6}};

static uint8_t Mem_GetBlockFlags(const Block_Number_t BlockNum)
{
	const Block_Number_t BlockIndex    = (BlockNum >> 2);
	const uint8_t        FlagMask      = Mem_MemData.FlagMaskLookupMask[BlockNum & 0x03];
	const uint8_t        FlagMaskShift = Mem_MemData.FlagMaskLookupNum[BlockNum & 0x03];

	return ((Mem_MemData.Mem_Block_Flags[BlockIndex] & FlagMask) >> FlagMaskShift);
}

static void Mem_SetBlockFlags(const Block_Number_t BlockNum, const uint8_t Flags)
{
	const Block_Number_t BlockIndex    = (BlockNum >> 2);
	const uint8_t        FlagMask      = Mem_MemData.FlagMaskLookupMask[BlockNum & 0x03];
	const uint8_t        FlagMaskShift = Mem_MemData.FlagMaskLookupNum[BlockNum & 0x03];

	Mem_MemData.Mem_Block_Flags[BlockIndex] &= ~FlagMask;
	Mem_MemData.Mem_Block_Flags[BlockIndex] |= (Flags << FlagMaskShift);
}

static inline void Mem_Defrag(void)
{
	uint8_t        FreeStartBlock = 0;
	char*          FreeStartPtr   = NULL;
	char*          UsedStartPtr   = NULL;
	Block_Number_t CurrBlock;
	
	for (CurrBlock = 0; CurrBlock < NUM_BLOCKS; CurrBlock++)
	{
		if (!(Mem_GetBlockFlags(CurrBlock) & BLOCK_USED_MASK))
		{
			FreeStartPtr   = &Mem_MemData.Mem_Heap[CurrBlock * BLOCK_SIZE];
			FreeStartBlock = CurrBlock;
			break;
		}
	}
	
	if (FreeStartPtr == NULL)
	  return;

	while (++CurrBlock < NUM_BLOCKS)
	{
		uint8_t CurrBlockFlags = Mem_GetBlockFlags(CurrBlock);
	
		if (CurrBlockFlags & BLOCK_USED_MASK)
		{
			UsedStartPtr = &Mem_MemData.Mem_Heap[CurrBlock * BLOCK_SIZE];
		
			for (Handle_Number_t HandleNum = 0; HandleNum < NUM_HANDLES; HandleNum++)
			{
				if (Mem_MemData.Mem_Handles[HandleNum] == UsedStartPtr)
				{
					Mem_MemData.Mem_Handles[HandleNum] = FreeStartPtr;
					break;
				}
			}

			memcpy(FreeStartPtr, UsedStartPtr, BLOCK_SIZE);
			FreeStartPtr += BLOCK_SIZE;
			  
			Mem_SetBlockFlags(FreeStartBlock++, CurrBlockFlags);
			Mem_SetBlockFlags(CurrBlock, 0);
		}
	}
}

static inline bool Mem_FindFreeBlocks(uint8_t* const RetStartPtr, const Block_Number_t Blocks)
{
	Block_Number_t FreeInCurrSec = 0;

	for (Block_Number_t CurrBlock = 0; CurrBlock < NUM_BLOCKS; CurrBlock++)
	{
		if (Mem_GetBlockFlags(CurrBlock) & BLOCK_USED_MASK)
		  FreeInCurrSec = 0;
		else
		  FreeInCurrSec++;

		if (FreeInCurrSec >= Blocks)
		{
			*RetStartPtr = CurrBlock;
			return true;
		}
	}

	return false;
}

Mem_Handle_t Mem_Alloc(const Alloc_Size_t Bytes)
{
	Block_Number_t ReqBlocks = (Bytes / BLOCK_SIZE);
	Block_Number_t StartBlock;
	
	if (Bytes % BLOCK_SIZE)
	  ReqBlocks++;
	
	if (!(Mem_FindFreeBlocks(&StartBlock, ReqBlocks)))
	{
		Mem_Defrag();
		
		if (!(Mem_FindFreeBlocks(&StartBlock, ReqBlocks)))
		  return NULL;	
	}

	for (Block_Number_t UsedBlock = 0; UsedBlock < (ReqBlocks - 1); UsedBlock++)
	  Mem_SetBlockFlags((StartBlock + UsedBlock), (BLOCK_USED_MASK | BLOCK_LINKED_MASK));

	Mem_SetBlockFlags((StartBlock + (ReqBlocks - 1)), BLOCK_USED_MASK);
	
	for (Handle_Number_t AllocEntry = 0; AllocEntry < NUM_HANDLES; AllocEntry++)
	{
		Mem_Handle_t CurrHdl = (Mem_Handle_t)&Mem_MemData.Mem_Handles[AllocEntry];
	
		if (DEREF(CurrHdl, void*) == NULL)
		{
			DEREF(CurrHdl, void*) = &Mem_MemData.Mem_Heap[StartBlock * BLOCK_SIZE];
			return CurrHdl;
		}
	}

	return NULL;
}

Mem_Handle_t Mem_Realloc(Mem_Handle_t CurrAllocHdl, const Alloc_Size_t Bytes)
{
	Mem_Free(CurrAllocHdl);
	return Mem_Alloc(Bytes);
}

Mem_Handle_t Mem_Calloc(const Alloc_Size_t Bytes)
{
	Mem_Handle_t AllocHdl = Mem_Alloc(Bytes);
	
	if (AllocHdl != NULL)
	  memset(DEREF(AllocHdl, void*), 0x00, Bytes);

	return AllocHdl;
}

void Mem_Free(Mem_Handle_t CurrAllocHdl)
{
	char*          MemBlockPtr = DEREF(CurrAllocHdl, char*);
	Block_Number_t CurrBlock   = ((uint16_t)(MemBlockPtr - Mem_MemData.Mem_Heap) / BLOCK_SIZE);
	uint8_t        CurrBlockFlags;

	if ((CurrAllocHdl == NULL) || (MemBlockPtr == NULL))
	  return;

	do
	{
		CurrBlockFlags = Mem_GetBlockFlags(CurrBlock);
		Mem_SetBlockFlags(CurrBlock, 0);

		CurrBlock++;
	}
	while (CurrBlockFlags & BLOCK_LINKED_MASK);
	
	*CurrAllocHdl = NULL;
}

Block_Number_t Mem_TotalFreeBlocks(void)
{
	Block_Number_t FreeBlocks = 0;
	
	for (Block_Number_t CurrBlock = 0; CurrBlock < NUM_BLOCKS; CurrBlock++)
	{
		if (!(Mem_GetBlockFlags(CurrBlock) & BLOCK_USED_MASK))
		  FreeBlocks++;
	}
	
	return FreeBlocks;
}

Handle_Number_t Mem_TotalFreeHandles(void)
{
	Handle_Number_t FreeHandles = 0;
	
	for (Handle_Number_t CurrHandle = 0; CurrHandle < NUM_HANDLES; CurrHandle++)
	{
		if (Mem_MemData.Mem_Handles[CurrHandle] == NULL)
		  FreeHandles++;
	}
	
	return FreeHandles;
}
