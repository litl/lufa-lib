/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "LowLevel.h"

volatile uint8_t USB_CurrentMode = USB_MODE_NONE;
         uint8_t USB_Options;

void USB_Init(const uint8_t Mode, const uint8_t Options)
{
	if (USB_IsInitialized)
	  USB_ShutDown();

	USB_INT_DISABLE(USB_INT_VBUS);

	if (Mode == USB_MODE_UID)
	{
		UHWCON |=  (1 << UIDE);

		USB_INT_CLEAR(USB_INT_IDTI);
		USB_INT_ENABLE(USB_INT_IDTI);
	}
	else if (Mode == USB_MODE_DEVICE)
	{
		UHWCON |=  (1 << UIMOD);
		
		USB_CurrentMode = USB_MODE_DEVICE;
	}
	else if (Mode == USB_MODE_HOST)			
	{
		UHWCON &= ~(1 << UIMOD);
		
		USB_CurrentMode = USB_MODE_HOST;
	}

	if (UHWCON & (1 << UIDE))
	  USB_CurrentMode = USB_GetUSBModeFromUID();

	USB_InitTaskPointer();
	USB_OTGPAD_On();
	
	USB_Options = Options;
		
	if (USB_CurrentMode == USB_MODE_DEVICE)
	  USB_INT_ENABLE(USB_INT_VBUS);
	else
	  USB_SetupInterface();
	
	sei();
}

void USB_ShutDown(void)
{
	USB_ResetInterface();

	USB_CurrentMode = USB_MODE_NONE;
	USB_Options     = 0;

	USB_Interface_Disable();
	USB_PLL_Off();
	USB_REG_Off();
	USB_OTGPAD_Off();

	USB_INT_DISABLE(USB_INT_IDTI);
	USB_INT_DISABLE(USB_INT_VBUS);
	UHWCON &= ~(1 << UIDE);
}

void USB_ResetInterface(void)
{
	if (USB_IsConnected)
	  RAISE_EVENT(USB_Disconnect);

	USB_INT_DisableAllInterrupts();
	Endpoint_ClearEndpoints();
	Pipe_ClearPipes();

	USB_HOST_VBUS_Off();
	USB_HOST_AutoVBUS_Off();
	USB_HOST_ManualVBUS_Disable();
	USB_HOST_HostModeOff();
	
	USB_Detach();

	USB_IsConnected         = false;
	USB_IsInitialized       = false;
	USB_HostState           = HOST_STATE_Unattached;
	USB_ConfigurationNumber = 0;
}

bool USB_SetupInterface(void)
{	
	USB_ResetInterface();

	USB_INT_DisableAllInterrupts();
	Endpoint_ClearEndpoints();
	Pipe_ClearPipes();
	
	if (UHWCON & (1 << UIDE))
	{
		USB_INT_ENABLE(USB_INT_IDTI);
		USB_CurrentMode = USB_GetUSBModeFromUID();
	}
	  
	if (USB_CurrentMode == USB_MODE_NONE)
	{
		RAISE_EVENT(USB_PowerOnFail, POWERON_ERR_NoUSBModeSpecified);
		return USB_SETUPINTERFACE_FAIL;
	}
	else if (USB_CurrentMode == USB_MODE_DEVICE)
	{
		if (USB_Options & USB_DEV_LOWSPEED)
		  USB_DEV_SetLowSpeed();
		else
		  USB_DEV_SetHighSpeed();
		  
		USB_INT_ENABLE(USB_INT_VBUS);
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
			RAISE_EVENT(USB_PowerOnFail, POWERON_ERR_EndpointCreationFailed);
			return USB_SETUPINTERFACE_FAIL;
		}

		USB_INT_ENABLE(USB_INT_SUSPEND);
		USB_INT_ENABLE(USB_INT_EORSTI);
	}
	else if (USB_CurrentMode == USB_MODE_HOST)
	{
		USB_Attach();
		USB_HOST_HostModeOn();

		if (USB_Options & USB_HOST_MANUALVBUS)
		{
			USB_INT_CLEAR(USB_INT_SRPI);
			USB_HOST_ManualVBUS_Enable();
		}
	}
	
	USB_InitTaskPointer();

	return USB_SETUPINTERFACE_OK;
}
