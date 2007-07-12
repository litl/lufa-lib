#include "LowLevel.h"

void USB_Init(const uint8_t Mode, const uint8_t Options)
{
	USB_Disable();
		
	if (Mode == USB_MODE_MIXED)
	{
		UHWCON &= ~(1 << UIDE);
	}
	else if (Mode == USB_MODE_DEVICE)
	{
		UHWCON |= ((1 << UIDE) | (1 << UIMOD));
	}
	else if (Mode == USB_MODE_HOST)			
	{
		UHWCON |= (1 << UIDE);
	}
		  
	USB_InitTaskPointer();
	USB_OTGPAD_On();
	
	if (Options & USB_DEV_LOWSPEED)
	  USB_DEV_SetLowSpeed();
	else
	  USB_DEV_SetHighSpeed();
	  
	USB_INT_VBUS_Enable();
}

void USB_Disable(void)
{
	USBConnected   = false;
	USBInitialized = false;
	
	if (USB_GetUSBMode() == USB_MODE_DEVICE)
	  USB_DEV_Detach();

	UHWCON &= ~((1 << UIDE) | (1 << UIMOD));			

	USB_OTGPAD_Off();
	USB_INT_VBUS_Disable();
	USB_Interface_Disable();
}

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
}
