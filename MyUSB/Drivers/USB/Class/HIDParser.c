/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "HIDParser.h"

/*
	TO ADD: Usage stack in ProcessHIDReport; add 1 to stack for each USAGE entry, pop off stack for each IOF
*/

uint8_t ProcessHIDReport(const uint8_t* ReportData, uint16_t ReportSize, HID_ReportInfo_t* const ParserData)
{
	HID_StateTable_t  StateTable[HID_STACK_DEPTH];
	HID_StateTable_t* CurrStateTable               = &StateTable[0];
	uint16_t          BitOffsetIn                  = 0x00;
	uint16_t          BitOffsetOut                 = 0x00;
#if defined(HID_ENABLE_FEATURE_PROCESSING)
	uint16_t          BitOffsetFeature             = 0x00;
#endif
	CollectionPath_t* CurrCollectionPath           = NULL;
	CollectionPath_t* ParentCollectionPath;

	memset((void*)ParserData, 0x00, sizeof(HID_ReportInfo_t)); 

	StateTable[0].ReportCount = 1;

	while (ReportSize)
	{
		uint32_t ReportItemData = 0;
		
		switch (*ReportData & DATA_SIZE_MASK)
		{
			case DATA_SIZE_4:
				ReportItemData = *((uint32_t*)(ReportData + 1));
				break;
			case DATA_SIZE_2:
				ReportItemData = *((uint16_t*)(ReportData + 1));
				break;
			case DATA_SIZE_1:
				ReportItemData = *((uint8_t*)(ReportData + 1));
				break;
		}

		switch (*ReportData & (TYPE_MASK | TAG_MASK))
		{
			case (TYPE_GLOBAL | GLOBAL_TAG_PUSH):
				if (CurrStateTable == &StateTable[HID_STACK_DEPTH])
				  return HID_PARSE_HIDStackOverflow;
	
				memcpy((CurrStateTable - 1),
				       CurrStateTable,
				       sizeof(HID_ReportItem_t));

				CurrStateTable++;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_POP):
				if (CurrStateTable == &StateTable[0])
				  return HID_PARSE_HIDStackUnderflow;
				  
				CurrStateTable--;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_USAGEPAGE):
				CurrStateTable->Attributes.Usage.Page = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_LOGICALMIN):
				CurrStateTable->Attributes.Logical.Minimum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_LOGICALMAX):
				CurrStateTable->Attributes.Logical.Maximum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_PHYSMIN):
				CurrStateTable->Attributes.Physical.Minimum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_PHYSMAX):
				CurrStateTable->Attributes.Physical.Maximum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_UNITEXP):
				CurrStateTable->Attributes.Unit.Exponent = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_UNIT):
				CurrStateTable->Attributes.Unit.Type = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_REPORTSIZE):
				CurrStateTable->Attributes.BitSize = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_REPORTCOUNT):
				CurrStateTable->ReportCount = ReportItemData;
				break;
			case (TYPE_LOCAL | LOCAL_TAG_USAGE):
				CurrStateTable->Attributes.Usage.Usage = ReportItemData;
				break;
			case (TYPE_LOCAL | LOCAL_TAG_USAGEMIN):
				CurrStateTable->Attributes.Usage.Minimum = ReportItemData;
				break;
			case (TYPE_LOCAL | LOCAL_TAG_USAGEMAX):
				CurrStateTable->Attributes.Usage.Maximum = ReportItemData;
				break;
			case (TYPE_MAIN | MAIN_TAG_COLLECTION):
				ParentCollectionPath = CurrCollectionPath;
			
				if (CurrCollectionPath == NULL)
				{
					CurrCollectionPath = &ParserData->CollectionPaths[0];
				}
				else
				{
					do
					{
						if (CurrCollectionPath == &ParserData->CollectionPaths[HID_MAX_COLLECTIONS])
						  return HID_PARSE_InsufficientCollectionPaths;
					
						CurrCollectionPath++;
					}
					while (CurrCollectionPath->Parent != NULL);
				}
				
				CurrCollectionPath->Parent = ParentCollectionPath;
				CurrCollectionPath->Usage  = ReportItemData;

				break;
			case (TYPE_MAIN | MAIN_TAG_ENDCOLLECTION):				
				CurrCollectionPath = CurrCollectionPath->Parent;

				break;
			case (TYPE_MAIN | MAIN_TAG_INPUT):
			case (TYPE_MAIN | MAIN_TAG_OUTPUT):
#if defined(HID_ENABLE_FEATURE_PROCESSING)
			case (TYPE_MAIN | MAIN_TAG_FEATURE):
#endif
				for (uint8_t ReportItemNum = 0; ReportItemNum < CurrStateTable->ReportCount; ReportItemNum++)
				{
					HID_ReportItem_t* CurrReportItem = &ParserData->ReportItems[ParserData->TotalReportItems];
				
#if defined(HID_INCLUDE_CONTANT_DATA_ITEMS)
					if (ReportItemData & IOF_CONSTANT)
					  break;
#endif

					if (ParserData->TotalReportItems == HID_MAX_REPORTITEMS)
					  return HID_PARSE_InsufficientReportItems;
				  
					memcpy((void*)&CurrReportItem->Attributes,
					       &CurrStateTable->Attributes,
					       sizeof(HID_ReportItem_Attributes_t));
						   
					CurrReportItem->ItemFlags = ReportItemData;
					
					CurrReportItem->CollectionPath = CurrCollectionPath;
						
					switch (*ReportData & TAG_MASK)
					{
						case MAIN_TAG_INPUT:
							CurrReportItem->ItemType = REPORT_ITEM_TYPE_In;
							CurrReportItem->BitOffset = BitOffsetIn;
								
							BitOffsetIn += CurrStateTable->Attributes.BitSize;
							
							break;
						case MAIN_TAG_OUTPUT:
							CurrReportItem->ItemType = REPORT_ITEM_TYPE_Out;
							CurrReportItem->BitOffset = BitOffsetOut;
								
							BitOffsetOut += CurrStateTable->Attributes.BitSize;
							
							break;
#if defined(HID_ENABLE_FEATURE_PROCESSING)
						case MAIN_TAG_FEATURE:
							CurrReportItem->ItemType = REPORT_ITEM_TYPE_Feature;						
							CurrReportItem->BitOffset = BitOffsetFeature;
								
							BitOffsetFeature += CurrStateTable->Attributes.BitSize;		

							break;
#endif
					}
					
					ParserData->TotalReportItems++;
				}
				
				break;
		}
	  
		if ((*ReportData & TYPE_MASK) == TYPE_MAIN)
		{
			CurrStateTable->Attributes.Usage.Usage   = 0;
			CurrStateTable->Attributes.Usage.Minimum = 0;
			CurrStateTable->Attributes.Usage.Maximum = 0;
		}
		
		switch (*ReportData & DATA_SIZE_MASK)
		{
			case DATA_SIZE_4:
				ReportSize -= 5;
				ReportData += 5;
				break;
			case DATA_SIZE_2:
				ReportSize -= 3;
				ReportData += 3;
				break;
			case DATA_SIZE_1:
				ReportSize -= 2;
				ReportData += 2;
				break;
			case DATA_SIZE_0:
				ReportSize -= 1;
				ReportData += 1;
				break;
		}
	}
	
	return HID_PARSE_Sucessful;
}

void GetReportItemInfo(const uint8_t* ReportData, HID_ReportItem_t* const ReportItem)
{
	uint16_t DataBitsRem  = ReportItem->Attributes.BitSize;
	uint16_t CurrentBit   = ReportItem->BitOffset;
	uint32_t BitMask      = (1 << 0);

	ReportItem->Value = 0;

	while (DataBitsRem--)
	{
		if (ReportData[CurrentBit / 8] & (1 << (CurrentBit % 8)))
		  ReportItem->Value |= BitMask;
		
		CurrentBit++;
		BitMask <<= 1;
	}
}

void SetReportItemInfo(uint8_t* const ReportData, const HID_ReportItem_t* ReportItem)
{
	uint16_t DataBitsRem  = ReportItem->Attributes.BitSize;
	uint16_t CurrentBit   = ReportItem->BitOffset;
	uint32_t BitMask      = (1 << 0);

	while (DataBitsRem--)
	{
		if (ReportItem->Value & (1 << (CurrentBit % 8)))
		  ReportData[CurrentBit / 8] |= BitMask;

		CurrentBit++;
		BitMask <<= 1;
	}
}
