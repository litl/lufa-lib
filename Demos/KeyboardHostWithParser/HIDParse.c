/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "HIDParse.h"

HID_ReportInfo_t HIDReportInfo;

uint8_t ProcessHIDReport(uint8_t* ReportData, uint16_t ReportSize)
{
	HID_StateTable_t  StateTable[HID_STACK_DEPTH];
	HID_StateTable_t* CurrStateTable = &StateTable[0];

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
				  return HID_PARSE_StackOverflow;
	
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
				  return HID_PARSE_StackUnderflow;
				  
				/* Set the current state table pointer to the previous state table in the stack */
				CurrStateTable--;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_USAGEPAGE):
				puts_P(PSTR("  UPAGE\r\n"));
				StateTable->Attributes.Usage.Page = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_LOGICALMIN):
				puts_P(PSTR("  LMIN\r\n"));
				StateTable->Attributes.Logical.Minimum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_LOGICALMAX):
				puts_P(PSTR("  LMAX\r\n"));
				StateTable->Attributes.Logical.Maximum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_PHYSMIN):
				puts_P(PSTR("  PMIN\r\n"));
				StateTable->Attributes.Physical.Minimum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_PHYSMAX):
				puts_P(PSTR("  PMIN\r\n"));
				StateTable->Attributes.Physical.Maximum = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_UNITEXP):
				puts_P(PSTR("  UEXP\r\n"));
				StateTable->Attributes.Unit.Exponent = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_UNIT):
				puts_P(PSTR("  UNIT\r\n"));
				StateTable->Attributes.Unit.Type = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_REPORTSIZE):
				puts_P(PSTR("  RSIZE\r\n"));
				StateTable->Attributes.BitSize = ReportItemData;
				break;
			case (TYPE_GLOBAL | GLOBAL_TAG_REPORTCOUNT):
				puts_P(PSTR("  RCNT\r\n"));
				StateTable->ReportCount = ReportItemData;
				break;
			case (TYPE_LOCAL | LOCAL_TAG_USAGE):
				puts_P(PSTR("  USAGE\r\n"));
				StateTable->Attributes.Usage.Usage = ReportItemData;			
				break;
			case (TYPE_LOCAL | LOCAL_TAG_USAGEMIN):
				puts_P(PSTR("  UMIN\r\n"));
				StateTable->Attributes.Usage.Minimum = ReportItemData;			
				break;
			case (TYPE_LOCAL | LOCAL_TAG_USAGEMAX):
				puts_P(PSTR("  UMAX\r\n"));
				StateTable->Attributes.Usage.Maximum = ReportItemData;			
				break;
			case (TYPE_MAIN | MAIN_TAG_INPUT):
			case (TYPE_MAIN | MAIN_TAG_OUTPUT):
				printf_P(PSTR("  IO(%d)\r\n"), StateTable->ReportCount);
				
				/* Loop through, creating report item structures based on the report count */
				for (uint8_t ReportItemNum = 0; ReportItemNum < StateTable->ReportCount; ReportItemNum++)
				{
					/* Check to see if table of report items is full */
					if (HIDReportInfo.TotalReportItems == HID_MAX_REPORTITEMS)
					  return HID_PARSE_InsufficientReportItems;
				  
				  	/* Copy state table data into a new report item structure */ 
					memcpy(&HIDReportInfo.ReportItems[HIDReportInfo.TotalReportItems].Attributes,
					       &CurrStateTable->Attributes,
					       sizeof(HID_ReportItem_Attributes_t));
	
					if ((*ReportData & TAG_MASK) == MAIN_TAG_INPUT)
					{
						/* Set the report type in the data structure to the appropriate type */
						HIDReportInfo.ReportItems[HIDReportInfo.TotalReportItems].ItemType = REPORT_ITEM_TYPE_In;
					
						/* Set the bit offset of the report type to the current overall offset */
						HIDReportInfo.ReportItems[HIDReportInfo.TotalReportItems].BitOffset = HIDReportInfo.BitOffsetIn;
							
						/* Increment the IN bit offset value in the state table */
						HIDReportInfo.BitOffsetIn += CurrStateTable->Attributes.BitSize;
					}
					else
					{
						/* Set the report type in the data structure to the appropriate type */
						HIDReportInfo.ReportItems[HIDReportInfo.TotalReportItems].ItemType = REPORT_ITEM_TYPE_Out;
					
						/* Set the bit offset of the report type to the current overall offset */
						HIDReportInfo.ReportItems[HIDReportInfo.TotalReportItems].BitOffset = HIDReportInfo.BitOffsetOut;
							
						/* Increment the OUT bit offset value in the state table */
						HIDReportInfo.BitOffsetOut += CurrStateTable->Attributes.BitSize;
					}
					
					/* Increment the total report items counter */
					HIDReportInfo.TotalReportItems++;
				}
				
				break;
			case (TYPE_MAIN | MAIN_TAG_COLLECTION):
				puts_P(PSTR("  COLS\r\n"));
				break;
			case (TYPE_MAIN | MAIN_TAG_ENDCOLLECTION):
				puts_P(PSTR("  COLE\r\n"));
				break;
			default:
				puts_P(PSTR("  UNK?\r\n"));			
		}
	  
		/* Store the report item size bitmask for later use */
		uint8_t DataSizeMask = (*ReportData & DATA_SIZE_MASK);
		
		/* Now skip over the data, ready to process the next report item */
		switch (DataSizeMask)
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

void GetReportItemInfo(uint8_t* ReportData, HID_ReportItem_t* ReportItem)
{
	uint16_t DataBitsRem  = ReportItem->Attributes.BitSize;
	uint16_t CurrentBit   = ReportItem->BitOffset;
	uint32_t BitNumber    = 0;

	/* Preset the report item value to 0 so that the bits can be set according to the report data */
	ReportItem->Value = 0;

	while (DataBitsRem--)
	{
		/* If the next data bit is set in the input report, set the corresponding bit in the report item value */
		if (ReportData[CurrentBit / 8] & (1 << (CurrentBit % 8)))
		  ReportItem->Value |= (1 << BitNumber);

		/* Increment both the bit number in the current byte and the current overall value bit counters */
		BitNumber++;
		CurrentBit++;
	}
}

void SetReportItemInfo(uint8_t* ReportData, HID_ReportItem_t* ReportItem)
{
	uint16_t DataBitsRem  = ReportItem->Attributes.BitSize;
	uint16_t CurrentBit   = ReportItem->BitOffset;
	uint32_t BitNumber    = 0;

	while (DataBitsRem--)
	{
		/* If the next data bit is set in the report item value, set the corresponding bit in the output report */
		if (ReportItem->Value & (1 << (CurrentBit % 8)))
		  ReportData[CurrentBit / 8] |= (1 << BitNumber);

		/* Increment both the bit number in the current byte and the current overall value bit counters */
		BitNumber++;
		CurrentBit++;
	}
}
