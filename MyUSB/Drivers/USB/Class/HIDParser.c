/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "HIDParser.h"

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

	/* Reset the entire HID info structure */
	memset((void*)ParserData, 0x00, sizeof(HID_ReportInfo_t)); 

	/* One report count by default unless told otherwise */
	StateTable[0].ReportCount = 1;

	while (ReportSize)
	{
		uint32_t ReportItemData = 0;
		
		/* If data to load, store it into the state table as appropriate */
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

		/* Parse depending on the next report item's type and tag */
		switch (*ReportData & (TYPE_MASK | TAG_MASK))
		{
			case (TYPE_GLOBAL | GLOBAL_TAG_PUSH):
				puts_P(PSTR("  PUSH\r\n"));
			
				/* Check to ensure space on the State Table stack */
				if (CurrStateTable == &StateTable[HID_STACK_DEPTH])
				  return HID_PARSE_HIDStackOverflow;
	
			  	/* Copy current stack item to next item in stack */
				memcpy((CurrStateTable - 1),
				       CurrStateTable,
				       sizeof(HID_ReportItem_t));

				/* Set the current state table pointer to the next state table in the stack */
				CurrStateTable++;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_POP):
				puts_P(PSTR("  POP\r\n"));

				/* Check to ensure stack is not at the first entry */
				if (CurrStateTable == &StateTable[0])
				  return HID_PARSE_HIDStackUnderflow;
				  
				/* Set the current state table pointer to the previous state table in the stack */
				CurrStateTable--;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_USAGEPAGE):
				puts_P(PSTR("  UPAGE\r\n"));
				CurrStateTable->Attributes.Usage.Page = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_LOGICALMIN):
				puts_P(PSTR("  LMIN\r\n"));
				CurrStateTable->Attributes.Logical.Minimum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_LOGICALMAX):
				puts_P(PSTR("  LMAX\r\n"));
				CurrStateTable->Attributes.Logical.Maximum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_PHYSMIN):
				puts_P(PSTR("  PMIN\r\n"));
				CurrStateTable->Attributes.Physical.Minimum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_PHYSMAX):
				puts_P(PSTR("  PMIN\r\n"));
				CurrStateTable->Attributes.Physical.Maximum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_UNITEXP):
				puts_P(PSTR("  UEXP\r\n"));
				CurrStateTable->Attributes.Unit.Exponent = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_UNIT):
				puts_P(PSTR("  UNIT\r\n"));
				CurrStateTable->Attributes.Unit.Type = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_REPORTSIZE):
				puts_P(PSTR("  RSIZE\r\n"));
				CurrStateTable->Attributes.BitSize = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_REPORTCOUNT):
				puts_P(PSTR("  RCNT\r\n"));
				CurrStateTable->ReportCount = ReportItemData;
				break;
			case (TYPE_LOCAL | LOCAL_TAG_USAGE):
				puts_P(PSTR("  USAGE\r\n"));
				CurrStateTable->Attributes.Usage.Usage = ReportItemData;
				break;
			case (TYPE_LOCAL | LOCAL_TAG_USAGEMIN):
				puts_P(PSTR("  UMIN\r\n"));
				CurrStateTable->Attributes.Usage.Minimum = ReportItemData;
				break;
			case (TYPE_LOCAL | LOCAL_TAG_USAGEMAX):
				puts_P(PSTR("  UMAX\r\n"));
				CurrStateTable->Attributes.Usage.Maximum = ReportItemData;
				break;
			case (TYPE_MAIN | MAIN_TAG_COLLECTION):
				puts_P(PSTR("  COLS\r\n"));
				
				/* Save the parent path pointer of the current collection */
				CollectionPath_t* ParentCollectionPath = CurrCollectionPath;
				
				/* Check if the collection is the first collection item */
				if (CurrCollectionPath == NULL)
				{
					/* Set collection path pointer to first collection path item */
					CurrCollectionPath = &ParserData->CollectionPaths[0];
				}
				else
				{
					/* Find next unused collection path */
					do
					{
						/* Check if the collection paths array is full */
						if (CurrCollectionPath == &ParserData->CollectionPaths[HID_MAX_COLLECTIONS])
						  return HID_PARSE_InsufficientCollectionPaths;
					
						/* Select next collections path item */
						CurrCollectionPath++;
					}
					while (CurrCollectionPath->Parent != NULL);
				}
				
				/* Set the new collection path details */
				CurrCollectionPath->Parent = ParentCollectionPath;
				CurrCollectionPath->Usage  = ReportItemData;

				break;
			case (TYPE_MAIN | MAIN_TAG_ENDCOLLECTION):
				puts_P(PSTR("  COLE\r\n"));
				
				/* Update current collection path to parent of the closed collection */
				CurrCollectionPath = CurrCollectionPath->Parent;

				break;
			case (TYPE_MAIN | MAIN_TAG_INPUT):
			case (TYPE_MAIN | MAIN_TAG_OUTPUT):
#if defined(HID_ENABLE_FEATURE_PROCESSING)
			case (TYPE_MAIN | MAIN_TAG_FEATURE):
#endif
				printf_P(PSTR("  IO(%d)\r\n"), CurrStateTable->ReportCount);
				
				/* Loop through, creating report item structures based on the report count */
				for (uint8_t ReportItemNum = 0; ReportItemNum < CurrStateTable->ReportCount; ReportItemNum++)
				{
					/* Create pointer to the current report item entry for convenience */
					HID_ReportItem_t* CurrReportItem = &ParserData->ReportItems[ParserData->TotalReportItems];
				
#if defined(HID_INCLUDE_CONTANT_DATA_ITEMS)
					/* Don't create report items for constant entries, only variable data */
					if (ReportItemData & IOF_CONSTANT)
					  break;
#endif

					/* Check to see if table of report items is full */
					if (ParserData->TotalReportItems == HID_MAX_REPORTITEMS)
					  return HID_PARSE_InsufficientReportItems;
				  
				  	/* Copy state table data into a new report item structure */ 
					memcpy((void*)&CurrReportItem->Attributes,
					       &CurrStateTable->Attributes,
					       sizeof(HID_ReportItem_Attributes_t));
						   
					/* Save the report item flags */
					CurrReportItem->ItemFlags = ReportItemData;
					
					/* Save the report item collection path */
					CurrReportItem->CollectionPath = CurrCollectionPath;
						
					switch (*ReportData & TAG_MASK)
					{
						case MAIN_TAG_INPUT:
							/* Set the report type in the data structure to the appropriate type */
							CurrReportItem->ItemType = REPORT_ITEM_TYPE_In;
						
							/* Set the bit offset of the report type to the current overall offset */
							CurrReportItem->BitOffset = BitOffsetIn;
								
							/* Increment the IN bit offset value in the state table */
							BitOffsetIn += CurrStateTable->Attributes.BitSize;
							
							break;
						case MAIN_TAG_OUTPUT:
							/* Set the report type in the data structure to the appropriate type */
							CurrReportItem->ItemType = REPORT_ITEM_TYPE_Out;
						
							/* Set the bit offset of the report type to the current overall offset */
							CurrReportItem->BitOffset = BitOffsetOut;
								
							/* Increment the OUT bit offset value in the state table */
							BitOffsetOut += CurrStateTable->Attributes.BitSize;
							
							break;
#if defined(HID_ENABLE_FEATURE_PROCESSING)
						case MAIN_TAG_FEATURE:
							/* Set the report type in the data structure to the appropriate type */
							CurrReportItem->ItemType = REPORT_ITEM_TYPE_Feature;
						
							/* Set the bit offset of the report type to the current overall offset */
							CurrReportItem->BitOffset = BitOffsetFeature;
								
							/* Increment the FEATURE bit offset value in the state table */
							BitOffsetFeature += CurrStateTable->Attributes.BitSize;		

							break;
#endif
					}
					
					/* Increment the total report items counter */
					ParserData->TotalReportItems++;
				}
				
				break;
			default:
				puts_P(PSTR("  UNK?\r\n"));			
		}
	  
		if ((*ReportData & TYPE_MASK) == TYPE_MAIN)
		{
			/* Clear local parameters, which are not persistant between main items */
			CurrStateTable->Attributes.Usage.Usage   = 0;
			CurrStateTable->Attributes.Usage.Minimum = 0;
			CurrStateTable->Attributes.Usage.Maximum = 0;
		}
		
		/* Now skip over the data, ready to process the next report item */
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
	
	/* Parsing complete, return success */
	return HID_PARSE_Sucessful;
}

void GetReportItemInfo(const uint8_t* ReportData, HID_ReportItem_t* const ReportItem)
{
	uint16_t DataBitsRem  = ReportItem->Attributes.BitSize;
	uint16_t CurrentBit   = ReportItem->BitOffset;
	uint32_t BitMask      = (1 << 0);

	/* Preset the report item value to 0 so that the bits can be set according to the report data */
	ReportItem->Value = 0;

	while (DataBitsRem--)
	{
		/* If the next data bit is set in the input report, set the corresponding bit in the report item value */
		if (ReportData[CurrentBit / 8] & (1 << (CurrentBit % 8)))
		  ReportItem->Value |= BitMask;
		
		/* Increment the current overall value bit counter and shift the bit mask */
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
		/* If the next data bit is set in the report item value, set the corresponding bit in the output report */
		if (ReportItem->Value & (1 << (CurrentBit % 8)))
		  ReportData[CurrentBit / 8] |= BitMask;

		/* Increment the current overall value bit counter and shift the bit mask */
		CurrentBit++;
		BitMask <<= 1;
	}
}
