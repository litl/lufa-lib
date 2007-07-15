#include "LowLevel.h"

void USB_Init(const uint8_t Mode, const uint8_t Options)
{
	USB_PowerOff();
		
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

bool USB_PowerOn(void)
{
	if (USB_GetUSBMode() == USB_MODE_NONE)
	  return USB_POWERON_FAIL;
		
	USB_REG_On();
	USB_PLL_On();
			
	while (!(USB_PLL_IsReady()));
		
	USB_Interface_Disable();
	USB_Interface_Enable();
	USB_CLK_Unfreeze();

	if (USB_GetUSBMode() == USB_MODE_DEVICE)
	{
		if (Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, ENDPOINT_TYPE_CONTROL,
		                               ENDPOINT_DIR_OUT, ENDPOINT_SIZE_64, ENDPOINT_BANK_SINGLE)
		    == ENDPOINT_CONFIG_OK)
		{
			USB_DEV_Attach();
		}
		else
		{
			return USB_POWERON_FAIL;
		}
	}
			
	return USB_POWERON_OK;
}

void USB_PowerOff(void)
{
	USBConnected   = false;
	USBInitialized = false;
	
	if (USB_GetUSBMode() == USB_MODE_DEVICE)
	  USB_DEV_Detach();

	USB_OTGPAD_Off();
	USB_INT_VBUS_Disable();

	UHWCON &= ~((1 << UIDE) | (1 << UIMOD));			

	Endpoint_ClearEndpoints();

	USB_Interface_Disable();
	USB_PLL_Off();
	USB_REG_Off();
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
