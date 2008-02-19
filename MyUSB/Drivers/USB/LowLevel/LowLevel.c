/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "USBMode.h"

#define INCLUDE_FROM_LOWLEVEL_C
#include "LowLevel.h"

#if (!defined(USB_HOST_ONLY) && !defined(USB_DEVICE_ONLY))
volatile uint8_t USB_CurrentMode = USB_MODE_NONE;
#endif

#if !defined(USE_STATIC_OPTIONS)
volatile uint8_t USB_Options;
#endif

void USB_Init(
			   #if defined(USB_CAN_BE_BOTH)
			   const uint8_t Mode
			   #endif

			   #if (defined(USB_CAN_BE_BOTH) && !defined(USE_STATIC_OPTIONS))
			   ,
			   #elif (!defined(USB_CAN_BE_BOTH) && defined(USE_STATIC_OPTIONS))
			   void
			   #endif
			   
			   #if !defined(USE_STATIC_OPTIONS)
			   const uint8_t Options
			   #endif
			   )
{
	if (USB_IsInitialized)
	  USB_ShutDown();

	#if (!defined(USB_HOST_ONLY) && !defined(USB_DEVICE_ONLY))
	USB_CurrentMode = Mode;
	#endif
	
	#if defined(USB_DEVICE_ONLY)
		#if defined(USB_FULL_CONTROLLER)
		UHWCON |= (1 << UIMOD);
		#endif
	#elif defined(USB_HOST_ONLY)
	UHWCON &= ~(1 << UIMOD);
	#else
	if (Mode == USB_MODE_NONE)
	{
		RAISE_EVENT(USB_PowerOnFail, POWERON_ERROR_NoUSBModeSpecified);
		return;
	}
	else if (Mode == USB_MODE_UID)
	{
		UHWCON |=  (1 << UIDE);

		USB_INT_Clear(USB_INT_IDTI);
		USB_INT_Enable(USB_INT_IDTI);
		
		USB_CurrentMode = USB_GetUSBModeFromUID();
	}
	else if (Mode == USB_MODE_DEVICE)
	{
		UHWCON |=  (1 << UIMOD);
	}
	else if (Mode == USB_MODE_HOST)			
	{
		UHWCON &= ~(1 << UIMOD);
	}
	#endif

	#if defined(USB_CAN_BE_BOTH)
	USB_InitTaskPointer();
	#else
	USB_IsInitialized = true;
	USB_IsConnected   = false;
	#endif

	#if defined(USB_CAN_BE_HOST)
	USB_ControlPipeSize = PIPE_CONTROLPIPE_DEFAULT_SIZE;
	#endif
	
	#if defined(USB_FULL_CONTROLLER)
	USB_OTGPAD_On();
	#endif
	
	#if !defined(USE_STATIC_OPTIONS)
	USB_Options = Options;
	#endif

	#if defined(USB_DEVICE_ONLY)
		#if defined(USB_FULL_CONTROLLER)
		USB_INT_Enable(USB_INT_VBUS);
		#else
		USB_SetupInterface();
		USB_IsConnected = true;
		#endif
	#elif defined(USB_HOST_ONLY)
	USB_SetupInterface();
	#else
	if (USB_CurrentMode == USB_MODE_DEVICE)
	  USB_INT_Enable(USB_INT_VBUS);
	else
	  USB_SetupInterface();
	#endif
	
	sei();
}

void USB_ShutDown(void)
{
	USB_ResetInterface();

	#if (!defined(USB_HOST_ONLY) && !defined(USB_DEVICE_ONLY))
	USB_CurrentMode = USB_MODE_NONE;
	#endif
	
	#if !defined(USE_STATIC_OPTIONS)
	USB_Options     = 0;
	#endif

	USB_Interface_Disable();
	USB_PLL_Off();
	
	#if defined(USB_FULL_CONTROLLER)
	USB_OTGPAD_Off();
	#endif

	#if defined(USB_CAN_BE_BOTH)
	UHWCON &= ~(1 << UIDE);
	#endif
}

static void USB_ResetInterface(void)
{
	if (USB_IsConnected)
	  RAISE_EVENT(USB_Disconnect);

	USB_INT_DisableAllInterrupts();
	USB_INT_ClearAllInterrupts();

	#if defined(USB_CAN_BE_DEVICE)
	Endpoint_ClearEndpoints();
	#endif

	#if defined(USB_CAN_BE_HOST)
	Pipe_ClearPipes();
	USB_HOST_VBUS_Auto_Enable();
	USB_HOST_VBUS_Auto_Off();
	USB_HOST_HostMode_Off();
	#endif
	
	USB_Detach();
	
	USB_REG_Off();

	USB_IsConnected         = false;
	USB_IsInitialized       = false;

	#if defined(USB_CAN_BE_HOST)
	USB_HostState           = HOST_STATE_Unattached;
	#endif

	#if defined(USB_CAN_BE_DEVICE)
	USB_ConfigurationNumber = 0;
	#endif
}

void USB_SetupInterface(void)
{	
	USB_ResetInterface();

	#if defined(USB_CAN_BE_BOTH)
	if (UHWCON & (1 << UIDE))
	{
		USB_INT_Clear(USB_INT_IDTI);
		USB_INT_Enable(USB_INT_IDTI);
	}
	#endif

	#if defined(USB_CAN_BE_DEVICE)
	Endpoint_ClearEndpoints();
	#endif
	
	#if defined(USB_CAN_BE_HOST)
	Pipe_ClearPipes();
	#endif
	
	#if defined(USB_CAN_BE_BOTH)
	if (UHWCON & (1 << UIDE))
	{
		USB_INT_Enable(USB_INT_IDTI);
		USB_CurrentMode = USB_GetUSBModeFromUID();
	}
	#elif defined(USB_DEVICE_ONLY)
		#if defined(USB_FULL_CONTROLLER)
		USB_INT_Enable(USB_INT_VBUS);
		#endif
	#endif
		
	if (!(USB_Options & USB_OPT_REG_DISABLED))
	  USB_REG_On();
	
	USB_PLL_On();
			
	while (!(USB_PLL_IsReady()));
		
	USB_Interface_Reset();
	USB_CLK_Unfreeze();

	#if (defined(USB_CAN_BE_DEVICE) && defined(USB_FULL_CONTROLLER))
	if (USB_CurrentMode == USB_MODE_DEVICE)
	{
		if (USB_Options & USB_DEV_OPT_LOWSPEED)
		  USB_DEV_SetLowSpeed();
		else
		  USB_DEV_SetHighSpeed();
		  
		USB_INT_Enable(USB_INT_VBUS);
	}
	#endif

	#if defined(USB_CAN_BE_HOST)
	USB_INT_Enable(USB_INT_VBERRI);
	
	if (USB_CurrentMode == USB_MODE_HOST)
	  USB_Host_PrepareForDeviceConnect();
	#endif
	
	USB_Attach();

	#if defined(USB_DEVICE_ONLY)	
	USB_INT_Enable(USB_INT_SUSPEND);
	USB_INT_Enable(USB_INT_EORSTI);	
	#elif defined(USB_HOST_ONLY)
	USB_HOST_HostMode_On();
	#else
	if (USB_CurrentMode == USB_MODE_DEVICE)
	{
		USB_INT_Enable(USB_INT_SUSPEND);
		USB_INT_Enable(USB_INT_EORSTI);
	}
	else if (USB_CurrentMode == USB_MODE_HOST)
	{
		USB_HOST_HostMode_On();
	}
	#endif
	
	#if defined(USB_CAN_BE_BOTH)
	USB_InitTaskPointer();
	#else
	USB_IsInitialized = true;
	USB_IsConnected   = false;
	#endif
}
