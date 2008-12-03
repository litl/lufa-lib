/*
             LUFA Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _HID_REPORT_H_
#define _HID_REPORT_H_

	/* Includes: */
		#include <LUFA/Drivers/USB/Class/HIDParser.h>            // HID Class Report Parser
		
		#include "KeyboardHostWithParser.h"

	/* Macros: */
		#define USAGEPAGE_KEYBOARD             0x07

	/* Enums: */
		enum KeyboardHostWithParser_GetHIDReportDataCodes_t
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
		
			uint8_t                  TotalHIDReports;

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
