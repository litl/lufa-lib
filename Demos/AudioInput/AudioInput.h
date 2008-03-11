/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _AUDIO_OUTPUT_H_
#define _AUDIO_OUTPUT_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>

		#include "Descriptors.h"
				
		#include <MyUSB/Common/ButtLoadTag.h>           // PROGMEM tags readable by the ButtLoad project
		#include <MyUSB/Drivers/USB/USB.h>              // USB Functionality
		#include <MyUSB/Drivers/Board/Bicolour.h>       // Bicolour LEDs driver
		#include <MyUSB/Drivers/AT90USBXXX/ADC.h>       // ADC driver
		#include <MyUSB/Scheduler/Scheduler.h>          // Simple scheduler for task management

	/* Macros: */
		#define MIC_IN_ADC_CHANNEL               1
		
		#define SAMPLE_MAX_RANGE                 0xFFFF
		#define ADC_MAX_RANGE                    0x3FF

	/* Task Definitions: */
		TASK(USB_Audio_Task);

	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_CreateEndpoints);
		HANDLES_EVENT(USB_UnhandledControlPacket);

#endif
