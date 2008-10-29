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

/*
	Demonstration application for a TTL magnetic stripe reader (such as the
	Omron V3B-4K) by Denver Gingerich. See http://ossguy.com/ss_usb/ for the
	demonstration project website, including construction and support details.

	This example is based on the MyUSB Keyboard demonstration application,
	written by Denver Gingerich.
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

		#define MAG_T1_CLOCK_PIN    PORTC1
		#define MAG_T1_DATA_PIN     PORTC2
		#define MAG_T2_CLOCK_PIN    PORTC3
		#define MAG_T2_DATA_PIN     PORTC0
		#define MAG_T3_CLOCK_PIN    PORTC5
		#define MAG_T3_DATA_PIN     PORTC6
		#define MAG_CLS_PIN         PORTC4
		#define MAG_PIN             PINC
		#define MAG_DDR             DDRC
        #define MAG_PORT            PORTC
		#include "MagstripeHW.h"
		
	/* Task Definitions: */
		TASK(USB_Keyboard_Report);

	/* Macros: */
		#define REQ_GetReport      0x01
		#define REQ_GetIdle        0x02
		#define REQ_SetReport      0x09
		#define REQ_SetIdle        0x0A
		#define REQ_GetProtocol    0x03
		#define REQ_SetProtocol    0x0B

		/* *MAX_BITS: maximum number of bits per track
		 *
		 * ISO 7811 specifies a maximum recording density of
		 * 210 bits per inch.  The width of a card is 3.375", giving
		 * 3.375 * 210 = 709 bits per track.  Thus, 1024 bits should
		 * store a complete track with room to spare.  In practice,
		 * cards have been shown to use up to 707 bits per track so
		 * 709 bits per track is a very tight approximation.
		 *
		 * http://www.cyberd.co.uk/support/technotes/isocards.htm
		 */
		#define MAX_BITS           1024
		#define T1_MAX_BITS        MAX_BITS
		#define T2_MAX_BITS        MAX_BITS
		#define T3_MAX_BITS        MAX_BITS

		/* USB key codes, see HID Usage Tables 1.12 Section 10 (p. 53):
		 *  http://www.usb.org/developers/devclass_docs/Hut1_12.pdf
		 */
		#define KEY_NO_EVENT        0
		#define KEY_1              30
		#define KEY_0              39
		#define KEY_ENTER          40
		
	/* Type Defines: */
		typedef struct
		{
			uint8_t Modifier;
			uint8_t Reserved;
			uint8_t KeyCode[6];
		} USB_KeyboardReport_Data_t;
			
	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_ConfigurationChanged);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		
	/* Function Prototypes: */
		bool GetNextReport(USB_KeyboardReport_Data_t* ReportData);
		void ProcessLEDReport(uint8_t LEDReport);
		void SendKey(USB_KeyboardReport_Data_t* KeyboardReportData, uint8_t Key);
		void Send(USB_KeyboardReport_Data_t* KeyboardReportData, bool SendReport);

#endif
