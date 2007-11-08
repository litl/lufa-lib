/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "Host.h"

bool USB_Host_WaitMS(uint8_t MS)
{
	uint8_t MSRemaining = MS;

	while (MSRemaining)
	{
		if (USB_INT_OCCURRED(USB_INT_HSOFI))
		{
			USB_INT_CLEAR(USB_INT_HSOFI);
						
			MSRemaining--;
		}
					
		if (USB_INT_OCCURRED(USB_INT_DDISCI) || USB_INT_OCCURRED(USB_INT_BCERRI))
			return false;
	}

	return true;
}

void USB_Host_ResetDevice(void)
{
	USB_INT_DISABLE(USB_INT_SRPI);
	USB_INT_DISABLE(USB_INT_DCONNI);

	USB_HOST_ResetBus();

	USB_INT_CLEAR(USB_INT_SRPI);
	USB_INT_CLEAR(USB_INT_DCONNI);

	while(!(USB_HOST_ResetBus_IsDone()));

	USB_INT_ENABLE(USB_INT_SRPI);
	USB_INT_ENABLE(USB_INT_DCONNI);
}
