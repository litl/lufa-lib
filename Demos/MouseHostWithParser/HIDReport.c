/*
             LUFA Library
     Copyright (C) Dean Camera, 2009.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2009  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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

#include "HIDReport.h"

uint16_t         HIDReportSize;
HID_ReportInfo_t HIDReportInfo;

uint8_t GetHIDReportData(void)
{
	/* Create a buffer big enough to hold the entire returned HID report */
	uint8_t HIDReportData[HIDReportSize];
	
	USB_HostRequest = (USB_Host_Request_Header_t)
		{
			bmRequestType: (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE),
			bRequest:      REQ_GetDescriptor,
			wValue:        (DTYPE_Report << 8),
			wIndex:        0,
			wLength:       HIDReportSize,
		};

	/* Send control request to retrieve the HID report from the attached device */
	if (USB_Host_SendControlRequest(HIDReportData) != HOST_SENDCONTROL_Successful)
	  return ParseControlError;

	/* Send the HID report to the parser for processing */
	if (ProcessHIDReport(HIDReportData, HIDReportSize, &HIDReportInfo) != HID_PARSE_Sucessful)
	  return ParseError;
	
	return ParseSucessful;
}

void DumpHIDReportItems(void)
{
	/* Loop through each of the loaded HID report items in the processed item structure */
	for (uint8_t ItemIndex = 0; ItemIndex < HIDReportInfo.TotalReportItems; ItemIndex++)
	{
		/* Create pointer to the current report info structure */
		HID_ReportItem_t* RItem = &HIDReportInfo.ReportItems[ItemIndex];
		
		/* Print out each report item's details */
		printf_P(PSTR("  Item %d:\r\n"
					  "    Type:       %s\r\n"
					  "    Flags:      %d\r\n"
					  "    BitOffset:  %d\r\n"
					  "    BitSize:    %d\r\n"
					  "    Coll Ptr:   %d\r\n"
					  "    Coll Usage: %d\r\n"
					  "    Coll Prnt:  %d\r\n"
					  "    Usage Page: %d\r\n"
					  "    Usage:      %d\r\n"
					  "    Usage Min:  %d\r\n"
					  "    Usage Max:  %d\r\n"
					  "    Unit Type:  %d\r\n"
					  "    Unit Exp:   %d\r\n"
					  "    Log Min:    %d\r\n"
					  "    Log Max:    %d\r\n"
					  "    Phy Min:    %d\r\n"
					  "    Phy Max:    %d\r\n"), ItemIndex,
												 ((RItem->ItemType == REPORT_ITEM_TYPE_In) ? "IN" : "OUT"),
												 RItem->ItemFlags,
												 RItem->BitOffset,
												 RItem->Attributes.BitSize,
												 RItem->CollectionPath,
												 RItem->CollectionPath->Usage,
												 RItem->CollectionPath->Parent,
												 RItem->Attributes.Usage.Page,
												 RItem->Attributes.Usage.Usage,
												 RItem->Attributes.Usage.MinMax.Minimum,
												 RItem->Attributes.Usage.MinMax.Maximum,
												 RItem->Attributes.Unit.Type,
												 RItem->Attributes.Unit.Exponent,
												 RItem->Attributes.Logical.Minimum,
												 RItem->Attributes.Logical.Maximum,
												 RItem->Attributes.Physical.Minimum,
												 RItem->Attributes.Physical.Maximum);
	}
}
