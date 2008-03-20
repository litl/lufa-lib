/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "USBMode.h"
#if defined(USB_CAN_BE_DEVICE)

#include "Endpoint.h"

uint8_t USB_ControlEndpointSize = ENDPOINT_CONTROLEP_DEFAULT_SIZE;

void Endpoint_ConfigureEndpoint_P(const uint8_t EndpointNum,
                                  const uint8_t UECFG0Xdata,
                                  const uint8_t UECFG1Xdata)
{
	Endpoint_SelectEndpoint(EndpointNum);
	Endpoint_EnableEndpoint();
	
	UECFG0X = UECFG0Xdata;
	UECFG1X = ((UECFG1X & (1 << ALLOC)) | UECFG1Xdata);
	
	Endpoint_AllocateMemory();
}

void Endpoint_ClearEndpoints(void)
{
	UEINT = 0;

	for (uint8_t EPNum = 0; EPNum < ENDPOINT_MAXENDPOINTS; EPNum++)
	{
		Endpoint_ResetEndpoint(EPNum);
		Endpoint_SelectEndpoint(EPNum);	
		UEIENX = 0;
		UEINTX = 0;
		Endpoint_DeallocateMemory();
		Endpoint_DisableEndpoint();
	}
}

uint8_t Endpoint_Write_Stream_LE(void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)Buffer;
	
	while (Length)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_FIFOCON_Clear();

			while (!(Endpoint_ReadWriteAllowed()))
			{
				if (Endpoint_IsStalled())
				  return ENDPOINT_RWSTREAM_ERROR_EndpointStalled;
				else if (!(USB_IsConnected))
				  return ENDPOINT_RWSTREAM_ERROR_DeviceDisconnected;			
			}
		}

		Endpoint_Write_Byte(*(DataStream++));
		Length--;
		
		if (Endpoint_IsStalled())
		  return ENDPOINT_RWSTREAM_ERROR_EndpointStalled;
		else if (!(USB_IsConnected))
		  return ENDPOINT_RWSTREAM_ERROR_DeviceDisconnected;
	}
	
	return ENDPOINT_RWSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Write_Stream_BE(void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)(Buffer + Length - 1);
	
	while (Length)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_FIFOCON_Clear();

			while (!(Endpoint_ReadWriteAllowed()))
			{
				if (Endpoint_IsStalled())
				  return ENDPOINT_RWSTREAM_ERROR_EndpointStalled;
				else if (!(USB_IsConnected))
				  return ENDPOINT_RWSTREAM_ERROR_DeviceDisconnected;			
			}
		}
		
		Endpoint_Write_Byte(*(DataStream--));
		Length--;
		
		if (Endpoint_IsStalled())
		  return ENDPOINT_RWSTREAM_ERROR_EndpointStalled;
		else if (!(USB_IsConnected))
		  return ENDPOINT_RWSTREAM_ERROR_DeviceDisconnected;
	}
	
	return ENDPOINT_RWSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Read_Stream_LE(void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)Buffer;
	
	while (Length)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_FIFOCON_Clear();

			while (!(Endpoint_ReadWriteAllowed()))
			{
				if (Endpoint_IsStalled())
				  return ENDPOINT_RWSTREAM_ERROR_EndpointStalled;
				else if (!(USB_IsConnected))
				  return ENDPOINT_RWSTREAM_ERROR_DeviceDisconnected;			
			}
		}
		
		*(DataStream++) = Endpoint_Read_Byte();
		Length--;
		
		if (Endpoint_IsStalled())
		  return ENDPOINT_RWSTREAM_ERROR_EndpointStalled;
		else if (!(USB_IsConnected))
		  return ENDPOINT_RWSTREAM_ERROR_DeviceDisconnected;
	}
	
	return ENDPOINT_RWSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Read_Stream_BE(void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)(Buffer + Length - 1);
	
	while (Length)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_FIFOCON_Clear();

			while (!(Endpoint_ReadWriteAllowed()))
			{
				if (Endpoint_IsStalled())
				  return ENDPOINT_RWSTREAM_ERROR_EndpointStalled;
				else if (!(USB_IsConnected))
				  return ENDPOINT_RWSTREAM_ERROR_DeviceDisconnected;			
			}
		}
		
		*(DataStream--) = Endpoint_Read_Byte();
		Length--;
		
		if (Endpoint_IsStalled())
		  return ENDPOINT_RWSTREAM_ERROR_EndpointStalled;
		else if (!(USB_IsConnected))
		  return ENDPOINT_RWSTREAM_ERROR_DeviceDisconnected;
	}
	
	return ENDPOINT_RWSTREAM_ERROR_NoError;
}

#endif
