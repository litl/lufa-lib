/*
	Keyboard demonstration application by Denver Gingerich.

	This example is based on the MyUSB Mouse demonstration application,
	written by Dean Camera.
*/

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <MyUSB/Drivers/USB/USB.h>

		#include <avr/pgmspace.h>

	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Header_t               Header;
				
			uint16_t                              HIDSpec;
			uint8_t                               CountryCode;
		
			uint8_t                               TotalHIDDescriptors;

			uint8_t                               HIDReportType;
			uint16_t                              HIDReportLength;
		} USB_Descriptor_HID_t;

		typedef struct
		{
			uint8_t                               ReportData[63];
		} USB_Descriptor_HID_Keyboard_Report_t;

		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            Interface;
			USB_Descriptor_HID_t                  KeyboardHID;
	        USB_Descriptor_Endpoint_t             KeyboardEndpoint;
	        USB_Descriptor_Endpoint_t             KeyboardLEDsEndpoint;
		} USB_Descriptor_Configuration_t;
					
	/* Macros: */
		#define KEYBOARD_EPNUM               1
		#define KEYBOARD_LEDS_EPNUM          2
		#define KEYBOARD_EPSIZE              8

		#define DTYPE_HID                    0x21
		#define DTYPE_Report                 0x22

	/* Function Prototypes: */
		bool USB_GetDescriptor(const uint8_t Type, const uint8_t Index, const uint16_t LanguageID,
		                       void** const DescriptorAddr, uint16_t* const DescriptorSize)
		                       ATTR_WARN_UNUSED_RESULT ATTR_WEAK ATTR_NON_NULL_PTR_ARG(4, 5);

#endif
