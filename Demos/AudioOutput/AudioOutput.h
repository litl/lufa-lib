/*
             MyUSB Library
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

#ifndef _AUDIO_OUTPUT_H_
#define _AUDIO_OUTPUT_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>

		#include "Descriptors.h"
		
		#include <MyUSB/Version.h>                    // Library Version Information
		#include <MyUSB/Common/ButtLoadTag.h>         // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality
		#include <MyUSB/Drivers/Board/LEDs.h>         // LEDs driver
		#include <MyUSB/Scheduler/Scheduler.h>        // Simple scheduler for task management
	
	/* Macros: */
		#if defined(USB_FULL_CONTROLLER) || defined(USB_MODIFIED_FULL_CONTROLLER)
			#define TCCRxA          TCCR3A
			#define TCCRxB          TCCR3B
			#define OCRxA           OCR3A
			#define OCRxB           OCR3B
			#define WGMx0           WGM30
			#define COMxA1          COM3A1
			#define COMxA0          COM3A0
			#define COMxB1          COM3B1
			#define COMxB0          COM3B0
			#define CSx0            CS30
		#else
			#define TCCRxA          TCCR1A
			#define TCCRxB          TCCR1B
			#define OCRxA           OCR1A
			#define OCRxB           OCR1B
			#define WGMx0           WGM10
			#define COMxA1          COM1A1
			#define COMxA0          COM1A0
			#define COMxB1          COM1B1
			#define COMxB0          COM1B0
			#define CSx0            CS10
		#endif
		
	/* Enums: */
		/** Enum for the possible status codes for passing to the UpdateStatus() function. */
		enum StatusCodes_t
		{
			Status_USBNotReady    = 0, /**< USB is not ready (disconnected from a USB host) */
			Status_USBEnumerating = 1, /**< USB interface is enumerating */
			Status_USBReady       = 2, /**< USB interface is connected and ready */
		};

	/* Task Definitions: */
		TASK(USB_Audio_Task);

	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_ConfigurationChanged);
		HANDLES_EVENT(USB_UnhandledControlPacket);
	
	/* Function Prototypes: */
		/** Function prototype for the UpdateStatus() routine, to display status changes to the user. */
		void UpdateStatus(uint8_t CurrentStatus);

#endif
