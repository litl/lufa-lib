/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
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
			RequestType: (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE),
			RequestData: REQ_GetDescriptor,
			Value:       (DTYPE_Report << 8),
			Index:       0,
			DataLength:  HIDReportSize,
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

		/* Toggle status LED to indicate busy */
		if (LEDs_GetLEDs() & LEDS_LED4)
		  LEDs_TurnOffLEDs(LEDS_LED4);
		else
		  LEDs_TurnOnLEDs(LEDS_LED4);
	}
}
