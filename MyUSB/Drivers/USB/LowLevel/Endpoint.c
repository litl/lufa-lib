/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
#include "Endpoint.h"

bool Endpoint_ConfigureEndpoint_P(const uint8_t EndpointNum,
                                  const uint8_t UECFG0Xdata,
                                  const uint8_t UECFG1Xdata)
{
	Endpoint_SelectEndpoint(EndpointNum);
	Endpoint_EnableEndpoint();
	
	UECFG0X = UECFG0Xdata;
	UECFG1X = ((UECFG1X & (1 << ALLOC)) | UECFG1Xdata);
	
	Endpoint_AllocateMemory();
	
	return Endpoint_IsConfigured();
}

void Endpoint_ClearEndpoints(void)
{
	for (uint8_t EPNum = 0; EPNum < ENDPOINT_MAXENDPOINTS; EPNum++)
	{
		Endpoint_SelectEndpoint(EPNum);
		Endpoint_DeallocateMemory();
		Endpoint_DisableEndpoint();
	}
}
#endif
