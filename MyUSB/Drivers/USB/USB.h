/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __USB_H__
#define __USB_H__

	/* Preprocessor Checks: */
		#if ((defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__)) && defined(USB_HOST_ONLY))
			#error USB_HOST_ONLY is not avaliable for the currently selected USB AVR model.
		#endif
		
		#if (!(defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB647__) \
		    || defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__)))
			#error The currently selected AVR model is not supported under the USB component of the MyUSB library.
		#endif

		#if (defined(USB_HOST_ONLY) && defined(USB_DEVICE_ONLY))
			#error USB_HOST_ONLY and USB_DEVICE_ONLY are mutually exclusive.
		#endif
		
	/* Includes: */
		#include "LowLevel/USBMode.h"
	
		#if defined(USB_CAN_BE_HOST)
			#include "HighLevel/ConfigDescriptor.h"
			#include "LowLevel/Host.h"
			#include "LowLevel/HostChapter9.h"
			#include "LowLevel/Pipe.h"
			#include "LowLevel/OTG.h"
		#endif
		
		#if defined(USB_CAN_BE_DEVICE)
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

