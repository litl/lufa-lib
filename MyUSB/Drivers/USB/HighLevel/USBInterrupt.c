#include "USBInterrupt.h"

ISR(USB_GEN_vect)
{
	if (USB_INT_VBUS_IntOccured() && USB_INT_VBUS_IsEnabled())
	{
		if (!(USBConnected) && USB_VBUS_GetStatus() && USBInitialized)
		{
			USB_EVENT_OnVBUSDetect();
			
			if (USB_PowerOn() == USB_POWERON_OK)
			{
				USBConnected = true;
				
				USB_EVENT_OnUSBConnect();
			}
		}
		else
		{
			USB_EVENT_OnUSBDisconnect();
		
			USBConnected = false;
		}
		
		USB_INT_VBUS_Reset();
	}
}
