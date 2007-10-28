/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "USBTask.h"

volatile bool      USB_IsConnected;
volatile bool      USB_IsInitialized;
         TaskPtr_t USB_TaskPtr;
volatile uint8_t   USB_HostState;

TASK(USB_USBTask)
{
	if (USB_IsInitialized)
	  (*USB_TaskPtr)();
}

void USB_InitTaskPointer(void)
{
	if (USB_CurrentMode != USB_MODE_NONE)
	{
		if (USB_CurrentMode == USB_MODE_DEVICE)
		  USB_TaskPtr = (TaskPtr_t)USB_DeviceTask;
		else
		  USB_TaskPtr = (TaskPtr_t)USB_HostTask;

		USB_IsInitialized = true;
	}
	else
	{
		USB_IsInitialized = false;
	}

	USB_IsConnected = false;
}

void USB_DeviceTask(void)
{
	if (USB_IsConnected)
	{
		Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);

		if (USB_IsSetupRecieved())
		  USB_DEVC9_ProcessControlPacket();
	}
}

void USB_HostTask(void)
{
	switch (USB_HostState)
	{
		case HOST_STATE_Unattached:
			USB_INT_CLEAR(USB_INT_BCERRI);

			if (!(USB_Options & USB_HOST_MANUALVBUS))
			  USB_HOST_AutoVBUS_On();

			if (USB_VBUS_GetStatus())
			  USB_HostState = HOST_STATE_Attached;

			break;
		case HOST_STATE_Attached:
			if (USB_INT_OCCURRED(USB_INT_SRPI) || USB_INT_OCCURRED(USB_INT_DCONNI))
			{
				USB_INT_CLEAR(USB_INT_SRPI);
				USB_INT_CLEAR(USB_INT_DCONNI);

				USB_INT_ENABLE(USB_INT_DDISCI);

				USB_IsConnected = true;
				RAISE_EVENT(USB_Connect);
				
				USB_HOST_SOFGeneration_Enable();
				
				if (USB_Host_WaitMS(100) == false)
				{
					USB_HostState = HOST_STATE_Unattached;
					break;
				}
				
				USB_INT_CLEAR(USB_INT_RSTI);
				USB_HOST_ResetDevice();

				while (!(USB_INT_OCCURRED(USB_INT_RSTI)));
				
				if (USB_Host_WaitMS(100) == false)
				{
					USB_HostState = HOST_STATE_Unattached;
					break;
				}				

				USB_HostState = HOST_STATE_Powered;
			}
			else if (USB_INT_OCCURRED(USB_INT_BCERRI) || USB_INT_OCCURRED(USB_INT_VBERRI))
			{
				if (USB_INT_OCCURRED(USB_INT_VBERRI))
				  RAISE_EVENT(USB_HostError, HOST_ERROR_VBusVoltageDip);
			
				USB_INT_CLEAR(USB_INT_VBERRI);
					
				USB_HOST_AutoVBUS_Off();				
			
				USB_HostState = HOST_STATE_Unattached;
			}
			
			break;
		case HOST_STATE_Powered:
			USB_Host_WaitMS(100);

			Pipe_ConfigurePipe(PIPE_CONTROLPIPE, PIPE_TYPE_CONTROL,
		                       PIPE_TOKEN_SETUP, PIPE_CONTROLPIPE,
							   PIPE_CONTROLPIPE_SIZE, PIPE_BANK_SINGLE);

			USB_HostState = HOST_STATE_Default;
		
			break;
		case HOST_STATE_Default:
			HostRequest.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE);
			HostRequest.bRequest      = REQ_GetDescriptor;
			HostRequest.wValue        = (DTYPE_Device << 8);
			HostRequest.wIndex        = 0;
			HostRequest.wLength       = 64;
			
			USB_Host_SendControlRequest(NULL);

			break;
		case HOST_STATE_Addressed:
		
			break;
		case HOST_STATE_Configured:
			
			break;
		case HOST_STATE_Suspended:
			
			break;
	}
}
