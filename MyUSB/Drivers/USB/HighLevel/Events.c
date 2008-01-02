/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#if ((defined(__AVR_AT90USB1286__) || (defined(__AVR_AT90USB646__))) && !(defined(USB_DEVICE_ONLY)))
	#define USB_DEVICE_ONLY
#endif

#define INCLUDE_FROM_EVENTS_C
#include "Events.h"

static void __stub (void)
{

}
