/*
             LUFA Library
     Copyright (C) Dean Camera, 2009.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2009  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include "USBMode.h"
#if defined(USB_CAN_BE_DEVICE)

#define  INCLUDE_FROM_ENDPOINT_C
#include "Endpoint.h"

#if !defined(NO_ENDPOINT_STREAMS)
static bool USB_EndpointStreamZLP[ENDPOINT_TOTAL_ENDPOINTS];
#endif

uint16_t    USB_EndpointSize[
#if !defined(NO_ENDPOINT_STREAMS)
                             ENDPOINT_TOTAL_ENDPOINTS
#else
                             1
#endif
                             ] = {
                                  #if defined(FIXED_CONTROL_ENDPOINT_SIZE)
                                  FIXED_CONTROL_ENDPOINT_SIZE
                                  #else
                                  ENDPOINT_CONTROLEP_DEFAULT_SIZE
                                  #endif
                                 };


#if !defined(STATIC_ENDPOINT_CONFIGURATION)
bool Endpoint_ConfigureEndpoint(const uint8_t  Number, const uint8_t Type, const uint8_t Direction,
			                    const uint16_t Size, const uint8_t Banks)
{
	Endpoint_SelectEndpoint(Number);
	Endpoint_EnableEndpoint();

	UECFG1X = 0;
	
#if !defined(NO_ENDPOINT_STREAMS)
	USB_EndpointSize[Number] = Endpoint_BytesToBankSize(Size);
#endif

	UECFG0X = ((Type << EPTYPE0) | Direction);
	UECFG1X = ((1 << ALLOC) | Banks | Endpoint_BytesToEPSizeMask(Size));

	return Endpoint_IsConfigured();
}
#else
bool Endpoint_ConfigureEndpointStatic(const uint8_t Number, const uint8_t UECFG0XData, const uint8_t UECFG1XData
#if !defined(NO_ENDPOINT_STREAMS)
                                      , const uint16_t Size
#endif
                                      )
{
	Endpoint_SelectEndpoint(Number);
	Endpoint_EnableEndpoint();

#if !defined(NO_ENDPOINT_STREAMS)
	USB_EndpointSize[Number] = Size;
#endif

	UECFG1X = 0;	

	UECFG0X = UECFG0XData;
	UECFG1X = UECFG1XData;

	return Endpoint_IsConfigured();
}
#endif

void Endpoint_ClearEndpoints(void)
{
	UEINT = 0;

	for (uint8_t EPNum = 0; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
	{
		Endpoint_SelectEndpoint(EPNum);	
		UEIENX = 0;
		UEINTX = 0;
		Endpoint_DeallocateMemory();
		Endpoint_DisableEndpoint();
	}
}

uint8_t Endpoint_WaitUntilReady(void)
{
	uint8_t TimeoutMSRem = USB_STREAM_TIMEOUT_MS;

	USB_INT_Clear(USB_INT_SOFI);

	while (!(Endpoint_ReadWriteAllowed()))
	{
		if (!(USB_IsConnected))
		  return ENDPOINT_READYWAIT_DeviceDisconnected;
		else if (Endpoint_IsStalled())
		  return ENDPOINT_READYWAIT_EndpointStalled;
			  
		if (USB_INT_HasOccurred(USB_INT_SOFI))
		{
			USB_INT_Clear(USB_INT_SOFI);

			if (!(TimeoutMSRem--))
			  return ENDPOINT_READYWAIT_Timeout;
		}
	}
	
	return ENDPOINT_READYWAIT_NoError;
}

#if !defined(NO_ENDPOINT_STREAMS)
void Endpoint_Finalize_Stream(void)
{
	Endpoint_ClearCurrentBank();

	if (USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()])
	{
		Endpoint_WaitUntilReady();
		Endpoint_ClearCurrentBank();
	}
	
	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = false;
}

void Endpoint_Finalize_Write_Control_Stream(void)
{
	Endpoint_ClearSetupIN();

	if (!(Endpoint_IsSetupOUTReceived()))
	{
		if (USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()])
		{
			while (!(Endpoint_IsSetupINReady()));
			Endpoint_ClearSetupIN();
		}
	}
	
	while (!(Endpoint_IsSetupOUTReceived()));
	Endpoint_ClearSetupOUT();
	
	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = false;
}

void Endpoint_Finalize_Read_Control_Stream(void)
{
	Endpoint_ClearSetupOUT();

	if (USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()])
	{
		while (!(Endpoint_IsSetupOUTReceived()))
		{
			if (Endpoint_IsSetupINReady())
			  break;
		}

		if (!(Endpoint_IsSetupINReady()))
		  Endpoint_ClearSetupOUT();
	}
	
	while (!(Endpoint_IsSetupINReady()));
	Endpoint_ClearSetupIN();
	
	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = false;
}

uint8_t Endpoint_Discard_Stream(uint16_t Length
#if !defined(NO_STREAM_CALLBACKS)
                                , uint8_t (* const Callback)(void)
#endif
								)
{
	uint8_t  ErrorCode;
	
	if ((ErrorCode = Endpoint_WaitUntilReady()))
	  return ErrorCode;

	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);

	while (Length)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_ClearCurrentBank();			

			#if !defined(NO_STREAM_CALLBACKS)
			if ((Callback != NULL) && (Callback() == STREAMCALLBACK_Abort))
			  return ENDPOINT_RWSTREAM_ERROR_CallbackAborted;
			#endif

			if ((ErrorCode = Endpoint_WaitUntilReady()))
			  return ErrorCode;
			  
			USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);
		}
		else
		{
			Endpoint_Discard_Byte();
			Length--;
		}
	}
	
	return ENDPOINT_RWSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Write_Stream_LE(const void* Buffer, uint16_t Length
#if !defined(NO_STREAM_CALLBACKS)
                                 , uint8_t (* const Callback)(void)
#endif
								 )
{
	uint8_t* DataStream   = (uint8_t*)Buffer;
	uint8_t  ErrorCode;
	
	if ((ErrorCode = Endpoint_WaitUntilReady()))
	  return ErrorCode;

	while (Length)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_ClearCurrentBank();
			
			#if !defined(NO_STREAM_CALLBACKS)
			if ((Callback != NULL) && (Callback() == STREAMCALLBACK_Abort))
			  return ENDPOINT_RWSTREAM_ERROR_CallbackAborted;
			#endif

			if ((ErrorCode = Endpoint_WaitUntilReady()))
			  return ErrorCode;
		}
		else
		{
			Endpoint_Write_Byte(*(DataStream++));
			Length--;
		}
	}

	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);
	
	return ENDPOINT_RWSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Write_Stream_BE(const void* Buffer, uint16_t Length
#if !defined(NO_STREAM_CALLBACKS)
                                 , uint8_t (* const Callback)(void)
#endif
								 )
{
	uint8_t* DataStream = (uint8_t*)(Buffer + Length - 1);
	uint8_t  ErrorCode;
	
	if ((ErrorCode = Endpoint_WaitUntilReady()))
	  return ErrorCode;

	while (Length)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_ClearCurrentBank();

			#if !defined(NO_STREAM_CALLBACKS)
			if ((Callback != NULL) && (Callback() == STREAMCALLBACK_Abort))
			  return ENDPOINT_RWSTREAM_ERROR_CallbackAborted;
			#endif

			if ((ErrorCode = Endpoint_WaitUntilReady()))
			  return ErrorCode;
		}
		else
		{
			Endpoint_Write_Byte(*(DataStream--));
			Length--;
		}
	}
	
	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);
	
	return ENDPOINT_RWSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Read_Stream_LE(void* Buffer, uint16_t Length
#if !defined(NO_STREAM_CALLBACKS)
                                 , uint8_t (* const Callback)(void)
#endif
								 )
{
	uint8_t* DataStream = (uint8_t*)Buffer;
	uint8_t  ErrorCode;
	
	if ((ErrorCode = Endpoint_WaitUntilReady()))
	  return ErrorCode;

	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);

	while (Length)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_ClearCurrentBank();

			#if !defined(NO_STREAM_CALLBACKS)
			if ((Callback != NULL) && (Callback() == STREAMCALLBACK_Abort))
			  return ENDPOINT_RWSTREAM_ERROR_CallbackAborted;
			#endif

			if ((ErrorCode = Endpoint_WaitUntilReady()))
			  return ErrorCode;

			USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);
		}
		else
		{
			*(DataStream++) = Endpoint_Read_Byte();
			Length--;
		}
	}
	
	return ENDPOINT_RWSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Read_Stream_BE(void* Buffer, uint16_t Length
#if !defined(NO_STREAM_CALLBACKS)
                                 , uint8_t (* const Callback)(void)
#endif
								 )
{
	uint8_t* DataStream = (uint8_t*)(Buffer + Length - 1);
	uint8_t  ErrorCode;
	
	if ((ErrorCode = Endpoint_WaitUntilReady()))
	  return ErrorCode;

	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);

	while (Length)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_ClearCurrentBank();

			#if !defined(NO_STREAM_CALLBACKS)
			if ((Callback != NULL) && (Callback() == STREAMCALLBACK_Abort))
			  return ENDPOINT_RWSTREAM_ERROR_CallbackAborted;
			#endif

			if ((ErrorCode = Endpoint_WaitUntilReady()))
			  return ErrorCode;

			USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);
		}
		else
		{
			*(DataStream--) = Endpoint_Read_Byte();
			Length--;
		}
	}
	
	return ENDPOINT_RWSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Write_Control_Stream_LE(const void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)Buffer;
	
	while (!(Endpoint_IsSetupINReady()));

	while (Length && !(Endpoint_IsSetupOUTReceived()))
	{
		if (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()])
		{
			Endpoint_ClearSetupIN();

			while (!(Endpoint_IsSetupINReady()))
			{
				if (Endpoint_IsSetupOUTReceived())
				  break;
			}
		}
		else
		{
			Endpoint_Write_Byte(*(DataStream++));
			Length--;
		}
	}

	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);
	
	if (Endpoint_IsSetupOUTReceived())
	  return ENDPOINT_RWCSTREAM_ERROR_HostAborted;
	
	return ENDPOINT_RWCSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Write_Control_Stream_BE(const void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)(Buffer + Length - 1);

	while (!(Endpoint_IsSetupINReady()));

	while (Length && !(Endpoint_IsSetupOUTReceived()))
	{
		if (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()])
		{
			Endpoint_ClearSetupIN();
			
			while (!(Endpoint_IsSetupINReady()))
			{
				if (Endpoint_IsSetupOUTReceived())
				  break;
			}
		}
		else
		{
			Endpoint_Write_Byte(*(DataStream--));
			Length--;
		}
	}

	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);
	
	if (Endpoint_IsSetupOUTReceived())
	  return ENDPOINT_RWCSTREAM_ERROR_HostAborted;
	
	return ENDPOINT_RWCSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Read_Control_Stream_LE(void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)Buffer;
	
	while (!(Endpoint_IsSetupOUTReceived()));

	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);

	while (Length)
	{
		if (!(Endpoint_BytesInEndpoint()))
		{
			Endpoint_ClearSetupOUT();
			while (!(Endpoint_IsSetupOUTReceived()));

			USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);
		}
		else
		{
			*(DataStream++) = Endpoint_Read_Byte();		
			Length--;
		}
	}
		
	return ENDPOINT_RWCSTREAM_ERROR_NoError;
}

uint8_t Endpoint_Read_Control_Stream_BE(void* Buffer, uint16_t Length)
{
	uint8_t* DataStream = (uint8_t*)(Buffer + Length - 1);
	
	while (!(Endpoint_IsSetupOUTReceived()));

	USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);

	while (Length)
	{
		if (!(Endpoint_BytesInEndpoint()))
		{
			Endpoint_ClearSetupOUT();
			while (!(Endpoint_IsSetupOUTReceived()));

			USB_EndpointStreamZLP[Endpoint_GetCurrentEndpoint()] = (Endpoint_BytesInEndpoint() == USB_EndpointSize[Endpoint_GetCurrentEndpoint()]);
		}
		else
		{
			*(DataStream--) = Endpoint_Read_Byte();		
			Length--;
		}
	}
		
	return ENDPOINT_RWCSTREAM_ERROR_NoError;
}
#endif

#endif
