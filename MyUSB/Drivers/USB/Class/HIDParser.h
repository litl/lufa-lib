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
		#include <string.h>

		#include "HIDReportData.h"

		#include "../../../Common/Common.h"

	/* Private Interface - For use in library only: */
		/* Macros: */
			#if !defined(HID_STACK_DEPTH)
				#define HID_STACK_DEPTH        5
			#endif
			
			#if !defined(HID_MAX_COLLECTIONS)
				#define HID_MAX_COLLECTIONS    5
			#endif

			#if !defined(HID_MAX_REPORTITEMS)
				#define HID_MAX_REPORTITEMS    30
			#endif

	/* Public Interface - May be used in end-application: */
		/* Enums: */
			enum HID_Types_t
			{
				REPORT_ITEM_TYPE_In               = 0,
				REPORT_ITEM_TYPE_Out              = 1,
				REPORT_ITEM_TYPE_Feature          = 2,
			};
			
			enum HID_Parse_ErrorCodes_t
			{
				HID_PARSE_Sucessful                   = 0,
				HID_PARSE_HIDStackOverflow            = 1,
				HID_PARSE_HIDStackUnderflow           = 2,
				HID_PARSE_InsufficientReportItems     = 3,
				HID_PARSE_InsufficientCollectionPaths = 4,
			};
		
		/* Type Defines: */		
			typedef struct CollectionPath
			{
				uint16_t                     Usage;
				struct CollectionPath*       Parent;
			} CollectionPath_t;
		
			typedef struct
			{
				uint32_t                     Minimum;
				uint32_t                     Maximum;
			} HID_MinMax_t;

			typedef struct
			{
				uint32_t                     Type;
				uint8_t                      Exponent;
			} HID_Unit_t;
			
			typedef struct
			{
				uint16_t                     Page;
				uint16_t                     Usage;
				uint16_t                     Minimum;
				uint16_t                     Maximum;
			} HID_Usage_t;

			typedef struct
			{
				uint8_t                      BitSize;
				
				HID_Usage_t                  Usage;
				HID_Unit_t                   Unit;
				HID_MinMax_t                 Logical;
				HID_MinMax_t                 Physical;	
			} HID_ReportItem_Attributes_t;
			
			typedef struct
			{
				uint16_t                     BitOffset;
				uint8_t                      ItemType;
				uint16_t                     ItemFlags;
				CollectionPath_t*            CollectionPath;

				HID_ReportItem_Attributes_t  Attributes;
							
				uint32_t                     Value;
			} HID_ReportItem_t;
			
			typedef struct
			{
				 HID_ReportItem_Attributes_t Attributes;
				 uint8_t                     ReportCount;
			} HID_StateTable_t;

			typedef struct
			{
				uint8_t                      TotalReportItems;
				HID_ReportItem_t             ReportItems[HID_MAX_REPORTITEMS];

				CollectionPath_t             CollectionPaths[HID_MAX_COLLECTIONS];
			} HID_ReportInfo_t;
			
		/* Function Prototypes: */
			uint8_t ProcessHIDReport(const uint8_t* ReportData, uint16_t ReportSize, HID_ReportInfo_t* const ParserData)
									  ATTR_NON_NULL_PTR_ARG(1, 3);
			void    GetReportItemInfo(const uint8_t* ReportData, HID_ReportItem_t* const ReportItem)
									  ATTR_NON_NULL_PTR_ARG(1, 2);
			void    SetReportItemInfo(uint8_t* const ReportData, const HID_ReportItem_t* ReportItem)
									  ATTR_NON_NULL_PTR_ARG(1, 2);

#endif
