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

#if !defined(USB_DEVICE_ONLY) && !defined(USB_HOST_ONLY) // All modes
         TaskPtr_t USB_TaskPtr;
#endif

#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
volatile uint8_t   USB_HostState;
#endif

TASK(USB_USBTask)
{
	#if defined(USB_HOST_ONLY) // USB_HOST_ONLY
		if (USB_IsInitialized)
		  USB_HostTask();
	#elif defined(USB_DEVICE_ONLY) // USB_DEVICE_ONLY
		USB_DeviceTask();
	#else  // All modes
		if (USB_IsInitialized)
		  (*USB_TaskPtr)();
	#endif
}

#if !defined(USB_DEVICE_ONLY) && !defined(USB_HOST_ONLY) // All modes
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
#endif

#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
void USB_DeviceTask(void)
{
	if (USB_IsConnected)
	{
		Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);

		if (Endpoint_IsSetupRecieved())
		  USB_Device_ProcessControlPacket();
	}
}
#endif

#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
void USB_HostTask(void)
{
	switch (USB_HostState)
	{
		case HOST_STATE_Unattached:
			if (!(USB_Options & USB_HOST_OPT_MANUALVBUS))
			  USB_HOST_AutoVBUS_On();

			if (USB_VBUS_GetStatus())
			{
				USB_INT_CLEAR(USB_INT_BCERRI);
				USB_HostState = HOST_STATE_Attached;
			}

			break;
		case HOST_STATE_Attached:
			if (USB_INT_OCCURRED(USB_INT_SRPI) || USB_INT_OCCURRED(USB_INT_DCONNI))
			{
				USB_INT_CLEAR(USB_INT_SRPI);
				USB_INT_CLEAR(USB_INT_DCONNI);

				USB_INT_ENABLE(USB_INT_DDISCI);

				RAISE_EVENT(USB_DeviceAttached);

				USB_IsConnected = true;
				RAISE_EVENT(USB_Connect);
					
				USB_HOST_SOFGeneration_Enable();
					
				if (USB_Host_WaitMS(100) != HOST_WAITERROR_Sucessful)
				{
					if (!(USB_Options & USB_HOST_OPT_MANUALVBUS))
					  USB_HOST_AutoVBUS_Off();

					RAISE_EVENT(USB_DeviceUnattached);

					USB_HostState = HOST_STATE_Unattached;
					break;
				}
					
				USB_HOST_ResetDevice();
					
				if (USB_Host_WaitMS(100) != HOST_WAITERROR_Sucessful)
				{
					if (!(USB_Options & USB_HOST_OPT_MANUALVBUS))
					  USB_HOST_AutoVBUS_Off();

					RAISE_EVENT(USB_DeviceUnattached);

					USB_HostState = HOST_STATE_Unattached;
					break;
				}

				USB_HostState = HOST_STATE_Powered;
			}
			else if (USB_INT_OCCURRED(USB_INT_BCERRI) || USB_INT_OCCURRED(USB_INT_VBERRI))
			{
				if (!(USB_Options & USB_HOST_OPT_MANUALVBUS))
				  USB_HOST_AutoVBUS_Off();

				if (USB_INT_OCCURRED(USB_INT_VBERRI))
				{
					RAISE_EVENT(USB_HostError, HOST_ERROR_VBusVoltageDip);
					RAISE_EVENT(USB_DeviceUnattached);
					USB_INT_CLEAR(USB_INT_VBERRI);
				}

				if (!(USB_INT_OCCURRED(USB_INT_BCERRI)))
				{
					RAISE_EVENT(USB_DeviceUnattached);
					USB_INT_CLEAR(USB_INT_BCERRI);
				}

				USB_HostState = HOST_STATE_Unattached;
			}
				
			break;
		case HOST_STATE_Powered:
			Pipe_ClearPipes();
		 
			Pipe_ConfigurePipe(PIPE_CONTROLPIPE, PIPE_TYPE_CONTROL,
							   PIPE_TOKEN_SETUP, PIPE_CONTROLPIPE,
							   PIPE_CONTROLPIPE_DEFAULT_SIZE, PIPE_BANK_SINGLE);

			USB_HostState = HOST_STATE_Default;
			
			break;
		case HOST_STATE_Default:
			USB_HostRequest.RequestType    = (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE);
			USB_HostRequest.RequestData    = REQ_GetDescriptor;
			USB_HostRequest.Value_HighByte = DTYPE_Device;
			USB_HostRequest.Value_LowByte  = 0;
			USB_HostRequest.Index          = 0;
			USB_HostRequest.Length         = USB_ControlPipeSize;

			uint8_t DataBuffer[offsetof(USB_Descriptor_Device_t, Endpoint0Size) + 1];
			
			if (USB_Host_SendControlRequest(DataBuffer, sizeof(DataBuffer))
			    == HOST_SEND_CONTROL_ERROR)
			{
				if (!(USB_Options & USB_HOST_OPT_MANUALVBUS))
				  USB_HOST_AutoVBUS_Off();

				RAISE_EVENT(USB_DeviceUnattached);

				USB_HostState = HOST_STATE_Unattached;
				break;
			}
			
			USB_ControlPipeSize = DataBuffer[offsetof(USB_Descriptor_Device_t, Endpoint0Size)];
			
			Pipe_DisablePipe();
			Pipe_DeallocateMemory();
			Pipe_ResetPipe(PIPE_CONTROLPIPE);
			
			Pipe_ConfigurePipe(PIPE_CONTROLPIPE, PIPE_TYPE_CONTROL,
			                   PIPE_TOKEN_SETUP, PIPE_CONTROLPIPE,
			                   USB_ControlPipeSize, PIPE_BANK_SINGLE);

			if ((Pipe_IsConfigured() == PIPE_CONFIG_FAIL) ||
			   (USB_Host_WaitMS(100) != HOST_WAITERROR_Sucessful))
			{
				if (!(USB_Options & USB_HOST_OPT_MANUALVBUS))
				  USB_HOST_AutoVBUS_Off();

				RAISE_EVENT(USB_DeviceUnattached);

				USB_HostState = HOST_STATE_Unattached;
				break;
			}

			USB_HostRequest.RequestType    = (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE);
			USB_HostRequest.RequestData    = REQ_SetAddress;
			USB_HostRequest.Value          = USB_HOST_DEVICEADDRESS;
			USB_HostRequest.Index          = 0;
			USB_HostRequest.Length         = 0;

			if (USB_Host_SendControlRequest(NULL, 0) == HOST_SEND_CONTROL_ERROR)
			{
				if (!(USB_Options & USB_HOST_OPT_MANUALVBUS))
				  USB_HOST_AutoVBUS_Off();

				RAISE_EVENT(USB_DeviceUnattached);

				USB_HostState = HOST_STATE_Unattached;
				break;
			}

			USB_HOST_SetDeviceAddress(USB_HOST_DEVICEADDRESS);
			
			USB_HostState = HOST_STATE_Addressed;

			break;
	}
}
#endif
