/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _HIDPARSE_H_
#define _HIDPARSE_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		
		#include "HIDReportData.h"

	/* Enums: */
		typedef enum
		{
			TYPE_IN      = 0,
			TYPE_OUT     = 1,
			TYPE_FEATURE = 2,
		} HID_Types_t;
	
	/* Type Defines: */
		typedef struct
		{
			uint32_t Minimum;
			uint32_t Maximum;
		} HID_MinMax_t;

		typedef struct
		{
			uint32_t Type;
			uint8_t  Exponent;
		} HID_Unit_t;
		
		typedef struct
		{
			uint16_t Page;
			uint16_t Usage;
		} HID_Usage_t;

		typedef struct
		{
			uint16_t          BitOffset;
			uint8_t           BitSize;
			uint8_t           ReportIndex;

			uint8_t           ItemType;
			uint8_t           Attribute;
			
			uint16_t          GlobalUsagePage;
			HID_Usage_t       Usage;

			HID_Unit_t        Unit;
			HID_MinMax_t      Logical;
			HID_MinMax_t      Physical;
			
			unsigned long     Value;
		} HID_ReportItem_t;

		typedef struct
		{
			uint8_t           TotalReports;

			uint8_t           TotalReportItems;
			HID_ReportItem_t  ReportItems[10];
		} HID_ReportInfo_t;

	/* External Variables: */
		extern HID_ReportInfo_t HIDReportInfo;

	/* Inline Functions: */
		static inline void ResetParser(void)
		{
			memset((void*)&HIDReportInfo, 0x00, sizeof(HID_ReportInfo_t));
		}

		static inline void SetReportNumber(uint8_t* ReportData, const uint8_t ReportNumber)
		{
			ReportData[0] = ReportNumber;
		}

		static inline uint8_t GetReportNumber(const uint8_t* ReportData)
		{
			return ReportData[0];
		}

	/* Function Prototypes: */
		void ResetParser(void);
		void ProcessHIDReport(uint8_t* ReportData, uint16_t ReportSize);

		void GetReportItemInfo(uint8_t* ReportData, HID_ReportItem_t* ReportItem);
		void SetReportItemInfo(uint8_t* ReportData, HID_ReportItem_t* ReportItem);

#endif
