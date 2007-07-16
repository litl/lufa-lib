#include "USBTask.h"

bool      USB_IsConnected;
bool      USB_IsInitialized;
TaskPtr_t USB_TaskPtr;

void USB_USBTask(void)
{
	if (USB_IsInitialized && USB_IsConnected)
	  (*USB_TaskPtr)();
}

void USB_InitTaskPointer(void)
{
	uint8_t USBMode = USB_GetUSBMode();
				
	if (USBMode != USB_MODE_NONE)
	{
		if (USBMode == USB_MODE_DEVICE)
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
	// Check for reset command here - if so reset control endpoint

   Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
   if (USB_IsSetupRecieved())
   {
		Bicolour_SetLed(2, BICOLOUR_LED2_RED);
   
		for (;;);
		
		USB_ClearSetupRecieved();
   }
}

void USB_HostTask(void)
{

}
