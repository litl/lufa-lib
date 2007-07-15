#include "USBTask.h"

#include "../../USBKEY/Bicolour.h"

bool      USBConnected;
bool      USBInitialized;
TaskPtr_t USBTaskPtr;

void USB_USBTask(void)
{
	if (USBInitialized && USBConnected)
	  (*USBTaskPtr)();

	/* START DEBUG */
	if (USBInitialized)
	{
		if (USBConnected)
		  Bicolour_SetLed(2, BICOLOUR_LED2_GREEN);
		else
		  Bicolour_SetLed(2, BICOLOUR_LED2_ORANGE);
	}
	/* END DEBUG */
}

void USB_InitTaskPointer(void)
{
	uint8_t USBMode = USB_GetUSBMode();
				
	if (USBMode != USB_MODE_NONE)
	{
		if (USBMode == USB_MODE_DEVICE)
		  USBTaskPtr = (TaskPtr_t)USB_DeviceTask;
		else
		  USBTaskPtr = (TaskPtr_t)USB_HostTask;

		USBInitialized = true;
	}
	else
	{
		USBInitialized = false;
	}

	USBConnected = false;
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
