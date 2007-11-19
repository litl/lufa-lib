/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
#include "Host.h"

USB_Host_Request_Header_t USB_HostRequest;

bool USB_Host_SendControlRequest(uint8_t* DataBuffer)
{
	uint8_t* HeaderByte     = (uint8_t*)&USB_HostRequest;
	uint8_t  DataLen        = USB_HostRequest.Length;
	bool     SOFGenEnabled  = USB_HOST_SOFGeneration_IsEnabled();
	uint8_t  TimeoutCounter;

	if (USB_Host_WaitMS(1) != HOST_WAITERROR_Sucessful)
	  return HOST_SEND_CONTROL_ERROR;

	USB_HOST_SOFGeneration_Enable();

	Pipe_SelectPipe(0);
	Pipe_SetToken(PIPE_TOKEN_SETUP);
	Pipe_ClearSetupSent();

	Pipe_Unfreeze();

	for (uint8_t i = 0; i < sizeof(USB_Host_Request_Header_t); i++)
	  USB_Host_Write_Byte(*(HeaderByte++));

	Pipe_SendPipeData();
	
	TimeoutCounter = 0;
	while (!(Pipe_IsSetupSent()))
	{
		if ((USB_Host_WaitMS(1) != HOST_WAITERROR_Sucessful) ||
		    (TimeoutCounter++ == USB_HOST_TIMEOUT_MS))
		{
			Pipe_Freeze();

			if (!(SOFGenEnabled))
			  USB_HOST_SOFGeneration_Disable();
	  
			return HOST_SEND_CONTROL_ERROR;
		}
	}
	
	Pipe_ClearSetupSent();
	Pipe_Freeze();

	if (USB_Host_WaitMS(1) != HOST_WAITERROR_Sucessful)
	{
		Pipe_Freeze();

		if (!(SOFGenEnabled))
		  USB_HOST_SOFGeneration_Disable();

		return HOST_SEND_CONTROL_ERROR;
	}

	if ((USB_HostRequest.RequestType & CONTROL_REQTYPE_DIRECTION) == REQDIR_DEVICETOHOST)
	{
		Pipe_SetInfiniteINRequests();
		Pipe_SetToken(PIPE_TOKEN_IN);
		
		while ((DataBuffer != NULL) && DataLen)
		{
			Pipe_Unfreeze();
		
			TimeoutCounter = 0;
			while (!(Pipe_In_IsRecieved()) && !(Pipe_IsSetupStalled()))
			{
				if ((USB_Host_WaitMS(1) != HOST_WAITERROR_Sucessful) ||
				    (TimeoutCounter++ == USB_HOST_TIMEOUT_MS))
				{
					Pipe_Freeze();

					if (Pipe_IsSetupStalled())
					  Pipe_ClearSetupStalled();

					if (!(SOFGenEnabled))
					  USB_HOST_SOFGeneration_Disable();

					return HOST_SEND_CONTROL_ERROR;
				}
			}
			
			if (Pipe_IsSetupStalled())
			{
				Pipe_ClearSetupStalled();
				break;
			}
			
			if (Pipe_BytesInPipe() == 0)
			  DataLen = 0;
			
			while (Pipe_BytesInPipe() && DataLen)
			{
				*(DataBuffer++) = USB_Host_Read_Byte();
				DataLen--;
			}
		
			Pipe_Freeze();
			Pipe_In_Clear();
			Pipe_ResetFIFO();
		}

		Pipe_SetToken(PIPE_TOKEN_OUT);
		Pipe_Unfreeze();
		Pipe_ResetFIFO();
		
		TimeoutCounter = 0;
		while (!(Pipe_Out_IsReady()) && !(Pipe_IsSetupStalled()))
		{
			if ((USB_Host_WaitMS(1) != HOST_WAITERROR_Sucessful) ||
			    (TimeoutCounter++ == USB_HOST_TIMEOUT_MS))
			{
				Pipe_Freeze();

				if (Pipe_IsSetupStalled())
				  Pipe_ClearSetupStalled();

				if (!(SOFGenEnabled))
				  USB_HOST_SOFGeneration_Disable();

				return HOST_SEND_CONTROL_ERROR;
			}
		}

		Pipe_Out_Clear();
	}
	else
	{
		Pipe_SetToken(PIPE_TOKEN_OUT);

		while ((DataBuffer != NULL) && DataLen)
		{
			Pipe_Unfreeze();
			
			if (DataLen <= USB_ControlPipeSize)
			{
				while (DataLen--)
				  USB_Host_Write_Byte(*(DataBuffer++));
			}
			else
			{
				for (uint16_t PipeByte = 0; PipeByte < USB_ControlPipeSize; PipeByte++)
				  USB_Host_Write_Byte(*(DataBuffer++));

				DataLen -= USB_ControlPipeSize;			
			}
			
			Pipe_SendPipeData();
			
			TimeoutCounter = 0;
			while (!(Pipe_Out_IsReady()) && !(Pipe_IsSetupStalled()))
			{
				if ((USB_Host_WaitMS(1) != HOST_WAITERROR_Sucessful) ||
				    (TimeoutCounter++ == USB_HOST_TIMEOUT_MS))
				{
					Pipe_Freeze();

					if (Pipe_IsSetupStalled())
					  Pipe_ClearSetupStalled();

					if (!(SOFGenEnabled))
					  USB_HOST_SOFGeneration_Disable();

					return HOST_SEND_CONTROL_ERROR;
				}
			}
			
			if (Pipe_IsSetupStalled())
			{
				Pipe_ClearSetupStalled();
				break;
			}
			
			Pipe_Out_Clear();
		}
		
		Pipe_Freeze();
		Pipe_SetToken(PIPE_TOKEN_IN);
		Pipe_Unfreeze();

		TimeoutCounter = 0;
		while (!(Pipe_In_IsRecieved()) && !(Pipe_IsSetupStalled()))
		{
			if ((USB_Host_WaitMS(1) != HOST_WAITERROR_Sucessful) ||
			    (TimeoutCounter++ == USB_HOST_TIMEOUT_MS))
			{
				Pipe_Freeze();

				if (Pipe_IsSetupStalled())
				  Pipe_ClearSetupStalled();

				if (!(SOFGenEnabled))
				  USB_HOST_SOFGeneration_Disable();

				return HOST_SEND_CONTROL_ERROR;
			}
		}

		Pipe_Freeze();
		Pipe_In_Clear();
		Pipe_ResetFIFO();
	}

	Pipe_Freeze();

	if (Pipe_IsSetupStalled())
	{
		Pipe_ClearSetupStalled();

		if (!(SOFGenEnabled))
		  USB_HOST_SOFGeneration_Disable();

		return HOST_SEND_CONTROL_ERROR;
	}
	
	if (!(SOFGenEnabled))
	  USB_HOST_SOFGeneration_Disable();

	return HOST_SEND_CONTROL_OK;
}
#endif
