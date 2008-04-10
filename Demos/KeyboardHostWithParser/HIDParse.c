/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "HIDParse.h"

HID_ReportInfo_t HIDReportInfo;

void ProcessHIDReport(uint8_t* ReportData, uint16_t ReportSize)
{
	HID_Usage_t       CurrUsage = {Page: 0, Usage: 0};
	HID_Unit_t        CurrUnit  = {Type: 0, Exponent: 0};
	HID_ReportItem_t* CurrItem  = &HIDReportInfo.ReportItems[0];
	uint8_t           ReportDataSize;
	
	while (ReportSize)
	{	
		if ((*ReportData & DATA_SIZE_MASK) == DATA_SIZE_4)
		  ReportDataSize = 4;
		else
		  ReportDataSize = (*ReportData & DATA_SIZE_MASK);
		
		switch (*ReportData)
		{
		
		}
		
		ReportData += (ReportDataSize + 1);
	}
}

void GetReportItemInfo(uint8_t* ReportData, HID_ReportItem_t* ReportItem)
{
	uint16_t DataBitsRem  = ReportItem->BitSize;
	uint16_t CurrentBit   = ReportItem->BitOffset;
	uint32_t BitNumber    = 0;

	while (DataBitsRem--)
	{
		if (ReportData[CurrentBit / 8] & (1 << (CurrentBit % 8)))
		  ReportItem->Value |= (1 << BitNumber);

		BitNumber++;
		CurrentBit++;
	}
}

void SetReportItemInfo(uint8_t* ReportData, HID_ReportItem_t* ReportItem)
{
	uint16_t DataBitsRem  = ReportItem->BitSize;
	uint16_t CurrentBit   = ReportItem->BitOffset;
	uint32_t BitNumber    = 0;

	while (DataBitsRem--)
	{
		if (ReportItem->Value & (1 << (CurrentBit % 8)))
		  ReportData[CurrentBit / 8] |= (1 << BitNumber);

		BitNumber++;
		CurrentBit++;
	}
}
