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

void USB_USBTask(void)
{
	if (USB_IsInitialized && USB_IsConnected)
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
	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
	if (USB_IsSetupRecieved())
	  USB_ProcessControlPacket();		
}

void USB_HostTask(void)
{

}
