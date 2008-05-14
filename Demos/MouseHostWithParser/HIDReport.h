/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _HID_REPORT_H_
#define _HID_REPORT_H_

	/* Includes: */
		#include <MyUSB/Drivers/USB/Class/HIDParser.h>            // HID Class Report Parser
		
		#include "MouseHostWithParser.h"

	/* Macros: */
		#define USAGE_PAGE_BUTTON           0x09
		#define USAGE_PAGE_GENERIC_DCTRL    0x01
		#define USAGE_X                     0x30
		#define USAGE_Y                     0x31

	/* Enums: */
		enum GetHIDReportDataCodes_t
		{
			ParseSucessful          = 0,
			ParseError              = 1,
			ParseControlError       = 2,
		};
		
	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Header_t  Header;
				
			uint16_t                 HIDSpec;
			uint8_t                  CountryCode;
		
			uint8_t                  TotalHIDDescriptors;

			uint8_t                  HIDReportType;
			uint16_t                 HIDReportLength;
		} USB_Descriptor_HID_t;

	/* External Variables: */
		extern uint16_t         HIDReportSize;
		extern HID_ReportInfo_t HIDReportInfo;

	/* Function Prototypes: */
		uint8_t GetHIDReportData(void);
		void    DumpHIDReportItems(void);
		
#endif
