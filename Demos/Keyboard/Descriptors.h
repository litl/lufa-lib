/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

/*
	Keyboard demonstration application by Denver Gingerich.

	This example is based on the MyUSB Mouse demonstration application,
	written by Dean Camera.

	Keyboard report descriptor is from the AVR-USB project, used with
	permission.
*/

#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

	/* Includes: */
		#include <MyUSB/Drivers/USB/USB.h>

	/* Type Defines */
		typedef struct
		{
			uint8_t Size;
			uint8_t Type;
		} USB_HIDDescriptor_Header_t;

		typedef struct
		{
			USB_HIDDescriptor_Header_t            Header;
				
			uint16_t                              HIDSpec;
			uint8_t                               CountryCode;
		
			uint8_t                               TotalHIDDescriptors;

			uint8_t                               HIDReportType;
			uint16_t                              HIDReportLength;
		} USB_Descriptor_HID_t;

		typedef struct
		{
			uint8_t                               ReportData[35];
		} USB_Descriptor_HID_Keyboard_Report_t;

		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            Interface;
			USB_Descriptor_HID_t                  KeyboardHID;
	        USB_Descriptor_Endpoint_t             KeyboardEndpoint;
		} USB_Descriptor_Configuration_t;
					
	/* Macros: */
		#define KEYBOARD_EPNUM               1
		#define KEYBOARD_EPSIZE              8

		#define DTYPE_HID                 0x21
		#define DTYPE_Report              0x22

		#define KEYBOARD_INTERFACE_NUMBER    0
		#define KEYBOARD_INTERFACE_ALTERNATE 0
		#define KEYBOARD_INTERFACE_ENDPOINTS 1
		#define KEYBOARD_INTERFACE_CLASS     0x03
		#define KEYBOARD_INTERFACE_SUBCLASS  0x00
		#define KEYBOARD_INTERFACE_PROTOCOL  0x01
		
		#define KEYBOARD_EPNUM               1
		#define KEYBOARD_EPSIZE              8

	/* Function Prototypes: */
	bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index,
	                       void** const DescriptorAddr, uint16_t* Size)
						   ATTR_WARN_UNUSED_RESULT ATTR_WEAK;

#endif
