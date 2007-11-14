/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
#include "Host.h"

uint8_t USB_Host_WaitMS(uint8_t MS)
{
	uint8_t MSRemaining    = MS;
	bool    SOFGenEnabled  = USB_HOST_SOFGeneration_IsEnabled();
	uint8_t ErrorCode      = HOST_WAITERROR_Sucessful;
	
	USB_INT_CLEAR(USB_INT_HSOFI);
	USB_HOST_SOFGeneration_Enable();

	while (MSRemaining)
	{
		if (USB_INT_OCCURRED(USB_INT_HSOFI))
		{
			USB_INT_CLEAR(USB_INT_HSOFI);
						
			MSRemaining--;
		}
					
		if (USB_INT_OCCURRED(USB_INT_DDISCI) ||
		    USB_INT_OCCURRED(USB_INT_BCERRI) ||
		    (USB_CurrentMode == USB_MODE_DEVICE))
		{
			ErrorCode = HOST_WAITERROR_DeviceDisconnect;
			
			break;
		}

		if (Pipe_IsError() == true)
		{
			Pipe_ClearError();
			ErrorCode = HOST_WAITERROR_PipeError;
			
			break;
		}
		
		if (Pipe_IsSetupStalled() == true)
		{
			Pipe_ClearSetupStalled();

			ErrorCode = HOST_WAITERROR_SetupStalled;
			
			break;			
		}
	}

	if (!(SOFGenEnabled))
	  USB_HOST_SOFGeneration_Disable();

	USB_INT_CLEAR(USB_INT_HSOFI);

	return ErrorCode;
}

void USB_Host_ResetDevice(void)
{
	USB_INT_DISABLE(USB_INT_SRPI);
	USB_INT_DISABLE(USB_INT_DCONNI);

	USB_HOST_ResetBus();

	USB_INT_CLEAR(USB_INT_SRPI);
	USB_INT_CLEAR(USB_INT_DCONNI);

	while (!(USB_HOST_ResetBus_IsDone()));

	USB_INT_ENABLE(USB_INT_SRPI);
	USB_INT_ENABLE(USB_INT_DCONNI);
}
#endif
