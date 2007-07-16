#include "LowLevel.h"

void USB_Init(const uint8_t Mode, const uint8_t Options)
{
	USB_PowerOff();
		
	if (Mode == USB_MODE_UID)
	{
		UHWCON |= (1 << UIDE);
	}
	else if (Mode == USB_MODE_DEVICE)
	{
		UHWCON &= ~(1 << UIDE);
		UHWCON |=  (1 << UIMOD);
	}
	else if (Mode == USB_MODE_HOST)			
	{
		UHWCON &= ~((1 << UIMOD) | (1 << UIDE));
	}

	USB_InitTaskPointer();
	USB_OTGPAD_On();
	
	if (Mode == USB_MODE_DEVICE)
	  USB_INT_ENABLE(USB_INT_VBUS);

	if (Options & USB_DEV_LOWSPEED)
	  USB_DEV_SetLowSpeed();
	else
	  USB_DEV_SetHighSpeed();

	sei();
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

		USB_INT_ENABLE(USB_INT_SUSPEND);
		USB_INT_ENABLE(USB_INT_EORSTI);
	}

	return USB_POWERON_OK;
}

void USB_PowerOff(void)
{
	USB_IsConnected   = false;
	USB_IsInitialized = false;
	
	if (USB_Interface_IsEnabled())
	  USB_EVENT_OnUSBDisconnect();
	
	if (USB_GetUSBMode() == USB_MODE_DEVICE)
	  USB_DEV_Detach();

	USB_OTGPAD_Off();

	USB_INT_DISABLE(USB_INT_VBUS);
	USB_INT_DISABLE(USB_INT_SUSPEND);

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
