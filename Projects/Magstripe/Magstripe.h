/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Denver Gingerich (denver [at] ossguy [dot] com)

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

/** \file
 *
 *  Header file for Magstripe.c.
 */

#ifndef _MAGSTRIPE_H_
#define _MAGSTRIPE_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>
		#include <string.h>
		#include <util/delay.h>

		#include "Descriptors.h"

		#include <MyUSB/Version.h>                    // Library Version Information
		#include <MyUSB/Common/ButtLoadTag.h>         // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/Board/LEDs.h>         // LEDs driver
		#include <MyUSB/Scheduler/Scheduler.h>        // Simple scheduler for task management

		#include "MagstripeHW.h"
		
	/* Task Definitions: */
		/** Task definition for the keyboard and magnetic card reading task. */
		TASK(USB_Keyboard_Report);

	/* Macros: */
		/** HID Class Specific Request to get the current HID report from the device. */
		#define REQ_GetReport      0x01

		/** HID Class Specific Request to get the current device idle count. */
		#define REQ_GetIdle        0x02

		/** HID Class Specific Request to set the current HID report to the device. */
		#define REQ_SetReport      0x09

		/** HID Class Specific Request to set the device's idle count. */
		#define REQ_SetIdle        0x0A

		/** HID Class Specific Request to get the current HID report protocol mode. */
		#define REQ_GetProtocol    0x03

		/** HID Class Specific Request to set the current HID report protocol mode. */
		#define REQ_SetProtocol    0x0B

		/** Maximum number of bits per track.
		 *
		 * ISO 7811 specifies a maximum recording density of
		 * 210 bits per inch.  The width of a card is 3.375", giving
		 * 3.375 * 210 = 709 bits per track.  Thus, 1024 bits should
		 * store a complete track with room to spare.  In practice,
		 * cards have been shown to use up to 707 bits per track so
		 * 709 bits per track is a very tight approximation.
		 *
		 * See http://www.cyberd.co.uk/support/technotes/isocards.htm for more information.
		 */
		#define MAX_BITS           1024
		
		/** Maximum number of bits for the first track on the card. */
		#define T1_MAX_BITS        MAX_BITS

		/** Maximum number of bits for the second track on the card. */
		#define T2_MAX_BITS        MAX_BITS

		/** Maximum number of bits for the third track on the card. */
		#define T3_MAX_BITS        MAX_BITS

		/** HID keyboard keycode to indicate no key currently pressed. */
		#define KEY_NO_EVENT        0
		
		/** HID keyboard keycode to indicate that the "1" key is currently pressed. */
		#define KEY_1              30

		/** HID keyboard keycode to indicate that the "0" key is currently pressed. */
		#define KEY_0              39

		/** HID keyboard keycode to indicate that the enter key is currently pressed. */
		#define KEY_ENTER          40
		
	/* Type Defines: */
		/** Type define for the keyboard report structure. This structure matches the report layout
		 *  given to the host in the HID Report descriptor, as well as matches the boot protocol report
		 *  structure. This means that this one report structure can be used in both Report and Boot Protocol
		 *  modes. */
		typedef struct
		{
			uint8_t Modifier; /**< Modifier byte, indicating pressed modifier keys such as CTRL or ALT */
			uint8_t Reserved; /**< Reserved for OEM use, always set to 0 */
			uint8_t KeyCode[6]; /**< Key code array for pressed keys - up to six can be given simultaneously */
		} USB_KeyboardReport_Data_t;
			
	/* Event Handlers: */
		/** Indicates that this module will catch the USB_Connect event when thrown by the library. */
		HANDLES_EVENT(USB_Connect);

		/** Indicates that this module will catch the USB_Disconnect event when thrown by the library. */
		HANDLES_EVENT(USB_Disconnect);

		/** Indicates that this module will catch the USB_ConfigurationChanged event when thrown by the library. */
		HANDLES_EVENT(USB_ConfigurationChanged);

		/** Indicates that this module will catch the USB_UnhandledControlPacket event when thrown by the library. */
		HANDLES_EVENT(USB_UnhandledControlPacket);
		
	/* Function Prototypes: */
		/* Prototype for the GetNextReport() function, to construct the next HID report for the host. */
		bool GetNextReport(USB_KeyboardReport_Data_t* ReportData);

		/** Prototype for the ProcessLEDReport() function, to process LED status reports from the host and display
		 *  them on the board LEDs.
		 */
		void ProcessLEDReport(uint8_t LEDReport);

		/** Prototype for the SendKey() function, to send a HID report indicating the given key is being pressed. */
		void SendKey(USB_KeyboardReport_Data_t* KeyboardReportData, uint8_t Key);

		/** Prototype for the Send() function, to send the given HID report to the host. */
		void Send(USB_KeyboardReport_Data_t* KeyboardReportData, bool SendReport);

#endif
