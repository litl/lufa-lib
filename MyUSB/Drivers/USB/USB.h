/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __USB_H__
#define __USB_H__

	#include <avr/io.h>

	/* Preprocessor Checks: */
		#if (defined(USB_HOST_ONLY) && defined(USB_DEVICE_ONLY))
			#error USB_HOST_ONLY and USB_DEVICE_ONLY are mutually exclusive.
		#endif
		
		#if ((defined(__AVR_AT90USB1286__) || (defined(__AVR_AT90USB646__))) && !(defined(USB_DEVICE_ONLY)))
			#define USB_DEVICE_ONLY
		#endif

	/* Includes: */
		#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
			#include "HighLevel/ConfigDescriptor.h"
			#include "LowLevel/Host.h"
			#include "LowLevel/HostChapter9.h"
			#include "LowLevel/Pipe.h"
		#endif
		
		#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
			#include "LowLevel/Device.h"
			#include "LowLevel/DevChapter9.h"
			#include "LowLevel/Endpoint.h"
		#endif
		
		#include "LowLevel/LowLevel.h"
		#include "HighLevel/USBTask.h"
		#include "HighLevel/USBInterrupt.h"
		#include "HighLevel/Events.h"
		#include "HighLevel/StdDescriptors.h"
		
#endif

