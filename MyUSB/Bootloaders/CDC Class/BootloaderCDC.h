/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _CDC_H_
#define _CDC_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/boot.h>
		#include <avr/eeprom.h>
		#include <stdbool.h>

		#include "Descriptors.h"

		#include <MyUSB/Drivers/USB/USB.h>                // USB Functionality
		#include <MyUSB/Drivers/Board/Joystick.h>         // Joystick driver
		#include <MyUSB/Drivers/Board/LEDs.h>             // LEDs driver

	/* Macros: */
		#define GET_LINE_CODING              0x21
		#define SET_LINE_CODING              0x20
		#define SET_CONTROL_LINE_STATE       0x22
		
		#define BOOTLOADER_VERSION_MAJOR     0x01
		#define BOOTLOADER_VERSION_MINOR     0x00
				
		#define BOOTLOADER_HWVERSION_MAJOR   0x01
		#define BOOTLOADER_HWVERSION_MINOR   0x00

		#define SOFTWARE_IDENTIFIER          "USBBOOT"

	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_CreateEndpoints);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		
	/* Type Defines: */
		typedef void (*AppPtr_t)(void) ATTR_NO_RETURN;

		typedef struct
		{
			uint32_t BaudRateBPS;
			uint8_t  CharFormat;
			uint8_t  ParityType;
			uint8_t  DataBits;
		} CDC_Line_Coding_t;
		
	/* Enums: */
		enum CDC_Line_Coding_Format_t
		{
			OneStopBit          = 0,
			OneAndAHalfStopBits = 1,
			TwoStopBits         = 2,
		};
		
		enum CDC_Line_Codeing_Parity_t
		{
			Parity_None         = 0,
			Parity_Odd          = 1,
			Parity_Even         = 2,
			Parity_Mark         = 3,
			Parity_Space        = 4,
		};

	/* Function Prototypes: */
		#if defined(INCLUDE_FROM_BOOTLOADERCDC_C)
			static void    ProgramReadMemoryBlock(const uint8_t Command);
			static uint8_t FetchNextCommandByte(void);
		#endif

	/* Tasks: */
		TASK(CDC_Task);

#endif
