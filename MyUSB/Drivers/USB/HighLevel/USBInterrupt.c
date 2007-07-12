#include "USBTask.h"

ISR(USB_GEN_vect)
{
	if (USB_INT_VBUS_IsEnabled() && (USB_VBUS_GetStatus() != USBConnected))
	{
		if (!(USBConnected) && USB_VBUS_GetStatus() && USBInitialized)
		{
			if (USB_PowerOn() == USB_POWERON_OK)
			  USBConnected = true;
		}
		else
		{
			USBConnected = false;
		}
		
		USB_INT_VBUS_Reset();
	}
}
