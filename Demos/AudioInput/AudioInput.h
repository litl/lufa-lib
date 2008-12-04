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

#ifndef _AUDIO_OUTPUT_H_
#define _AUDIO_OUTPUT_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>

		#include "Descriptors.h"
				
		#include <LUFA/Version.h>                      // Library Version Information
		#include <LUFA/Common/ButtLoadTag.h>           // PROGMEM tags readable by the ButtLoad project
		#include <LUFA/Drivers/USB/USB.h>              // USB Functionality
		#include <LUFA/Drivers/Board/LEDs.h>           // LEDs driver
		#include <LUFA/Drivers/AT90USBXXX/ADC.h>       // ADC driver
		#include <LUFA/Scheduler/Scheduler.h>          // Simple scheduler for task management

	/* Macros: */
		#define MIC_IN_ADC_CHANNEL               2
		
		#define SAMPLE_MAX_RANGE                 0xFFFF
		#define ADC_MAX_RANGE                    0x3FF

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
		void UpdateStatus(uint8_t CurrentStatus);
		
#endif
