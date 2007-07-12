#include "USBTask.h"

#include "../../USBKEY/Bicolour.h"

bool      USBConnected;
bool      USBInitialized;
TaskPtr_t USBTaskPtr;

TASK(USB_USBTask)
{
	if (USBInitialized && USBConnected)
	  (*USBTaskPtr)();
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
	Bicolour_SetLed(2, BICOLOUR_LED2_GREEN);
}

void USB_HostTask(void)
{
	Bicolour_SetLed(2, BICOLOUR_LED2_RED);
}
