/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "USBMode.h"
#if defined(USB_CAN_BE_HOST)

#include "Host.h"

uint8_t USB_Host_WaitMS(uint8_t MS)
{
	bool    BusSuspended = USB_Host_IsBusSuspended();
	uint8_t ErrorCode    = HOST_WAITERROR_Successful;
	
	USB_INT_Clear(USB_INT_HSOFI);
	USB_Host_ResumeBus();

	while (MS)
	{
		if (USB_INT_HasOccurred(USB_INT_HSOFI))
		{
			USB_INT_Clear(USB_INT_HSOFI);
			MS--;
		}
					
		if ((USB_IsConnected == false) || (USB_CurrentMode == USB_MODE_DEVICE))
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
		
		if (Pipe_IsStalled() == true)
		{
			Pipe_ClearStall();
			ErrorCode = HOST_WAITERROR_SetupStalled;
			
			break;			
		}
	}

	if (BusSuspended)
	  USB_Host_SuspendBus();

	return ErrorCode;
}

void USB_Host_ResetDevice(void)
{
	bool BusSuspended = USB_Host_IsBusSuspended();

	USB_INT_Disable(USB_INT_DDISCI);
	
	USB_Host_WaitMS(20);

	USB_Host_ResetBus();
	while (!(USB_Host_IsResetBusDone()));

	USB_INT_Clear(USB_INT_HSOFI);
	USB_Host_ResumeBus();	
	
	for (uint8_t MSRem = 10; MSRem != 0; MSRem--)
	{
		/* Workaround for powerless-pullup devices. After a USB bus reset,
		   all disconnection interrupts are supressed while a USB frame is
		   looked for - if it is found within 10ms, the device is still
		   present.                                                        */

		if (USB_INT_HasOccurred(USB_INT_HSOFI))
		{
			USB_INT_Clear(USB_INT_DDISCI);
			break;
		}
		
		_delay_ms(1);
	}

	if (BusSuspended)
	  USB_Host_SuspendBus();

	USB_INT_Enable(USB_INT_DDISCI);
}

void USB_Host_PrepareForDeviceConnect(void)
{
	USB_Host_VBUS_Auto_Off();
	USB_OTGPAD_Off();

	USB_Host_VBUS_Manual_Enable();
	USB_Host_VBUS_Manual_On();
	
	USB_INT_Enable(USB_INT_SRPI);
	USB_INT_Enable(USB_INT_BCERRI);

	USB_HostState = HOST_STATE_Unattached;
}
#endif
