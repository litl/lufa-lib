/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "LowLevel.h"

uint8_t USB_CurrentMode = USB_MODE_NONE;
uint8_t USB_Options;

void USB_Init(const uint8_t Mode, const uint8_t Options)
{
	USB_PowerOff();

	if (Mode == USB_MODE_UID)
	{
		UHWCON |= (1 << UIDE);
		
		USB_CurrentMode = USB_GetUSBModeFromUID();
	}
	else if (Mode == USB_MODE_DEVICE)
	{
		UHWCON &= ~(1 << UIDE);
		UHWCON |=  (1 << UIMOD);
		
		USB_CurrentMode = USB_MODE_DEVICE;
	}
	else if (Mode == USB_MODE_HOST)			
	{
		UHWCON &= ~((1 << UIMOD) | (1 << UIDE));
		
		USB_CurrentMode = USB_MODE_HOST;
	}

	USB_InitTaskPointer();
	USB_OTGPAD_On();
	
	if (USB_CurrentMode == USB_MODE_DEVICE)
	  USB_INT_ENABLE(USB_INT_VBUS);
	else
	  USB_PowerOn();
	
	USB_Options = Options;
	
	sei();
}

bool USB_PowerOn(void)
{
	if (USB_CurrentMode == USB_MODE_NONE)
	{
		return USB_POWERON_FAIL;
	}
	else if (USB_CurrentMode == USB_MODE_DEVICE)
	{
		if (USB_Options & USB_DEV_LOWSPEED)
		  USB_DEV_SetLowSpeed();
		else
		  USB_DEV_SetHighSpeed();
	}
	
	USB_REG_On();
	USB_PLL_On();
			
	while (!(USB_PLL_IsReady()));
		
	USB_Interface_Disable();
	USB_Interface_Enable();
	USB_CLK_Unfreeze();

	if (USB_CurrentMode == USB_MODE_DEVICE)
	{
		if (Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, ENDPOINT_TYPE_CONTROL,
		                               ENDPOINT_DIR_OUT, ENDPOINT_CONTROLEP_SIZE,
									   ENDPOINT_BANK_SINGLE) == ENDPOINT_CONFIG_OK)
		{
			USB_Attach();
		}
		else
		{
			return USB_POWERON_FAIL;
		}

		USB_INT_ENABLE(USB_INT_SUSPEND);
		USB_INT_ENABLE(USB_INT_EORSTI);
	}
	else if (USB_CurrentMode == USB_MODE_HOST)
	{
		if (Pipe_ConfigurePipe(PIPE_CONTROLPIPE, PIPE_TYPE_CONTROL,
		                       PIPE_TOKEN_SETUP, ENDPOINT_CONTROLEP,
							   PIPE_CONTROLPIPE_SIZE, PIPE_BANK_SINGLE)
							   == PIPE_CONFIG_OK)
		{
			USB_Attach();
			USB_HOST_HostModeOn();
		}
	}

	return USB_POWERON_OK;
}

void USB_PowerOff(void)
{
	USB_IsConnected   = false;
	USB_IsInitialized = false;
	
	if (USB_Interface_IsEnabled())
	  USB_EVENT_OnUSBDisconnect();
	
	USB_HOST_HostModeOff();
	USB_Detach();

	USB_OTGPAD_Off();

	USB_INT_DISABLE(USB_INT_VBUS);
	USB_INT_DISABLE(USB_INT_SUSPEND);

	USB_INT_CLEAR(USB_INT_WAKEUP);

	UHWCON &= ~((1 << UIDE) | (1 << UIMOD));			

	if (USB_CurrentMode == USB_MODE_DEVICE)
	  Endpoint_ClearEndpoints();
	else
	  Pipe_ClearPipes();

	USB_CLK_Freeze();
	USB_Interface_Disable();
	USB_PLL_Off();
	USB_REG_Off();
}
