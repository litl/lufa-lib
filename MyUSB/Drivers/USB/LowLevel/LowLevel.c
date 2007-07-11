#include "LowLevel.h"

uint8_t USB_GetUSBMode(void)
{
	if (UHWCON & (1 << UIDE))
	{
		if (UHWCON & (1 << UIMOD))
		  return USB_MODE_DEVICE;
		else
		  return USB_MODE_HOST;
	}
	else
	{
		if (USBSTA & (1 << ID))
		  return USB_MODE_DEVICE;
		else
		  return USB_MODE_HOST;
	}
	
	return USB_MODE_NONE;
}
