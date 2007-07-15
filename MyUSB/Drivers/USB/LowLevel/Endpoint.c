#include "Endpoint.h"

bool Endpoint_ConfigureEndpoint_PRV(const uint8_t EndpointNum,
                                    const uint8_t UECFG0Xdata,
                                    const uint8_t UECFG1Xdata)
{
	Endpoint_SelectEndpoint(EndpointNum);
	Endpoint_ActivateEndpoint();
	
	UECFG0X = UECFG0Xdata;
	UECFG1X = ((UECFG1X & (1 << ALLOC)) | UECFG1Xdata);
	
	Endpoint_AllocateMemory();
	
	return ((UESTA0X & (1 << CFGOK)) ? ENDPOINT_CONFIG_OK : ENDPOINT_CONFIG_FAIL);
}

void Endpoint_ClearEndpoints(void)
{
	for (uint8_t EPNum = 0; EPNum < 8; EPNum++)
	{
		Endpoint_SelectEndpoint(EPNum);
		Endpoint_DeallocateMemory();
		Endpoint_DeactivateEndpoint();
	}
}
