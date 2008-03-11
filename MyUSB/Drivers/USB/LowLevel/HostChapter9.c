/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "USBMode.h"
#if defined(USB_CAN_BE_HOST)

#define  INCLUDE_FROM_HOSTCHAPTER9_C
#include "HostChapter9.h"

USB_Host_Request_Header_t USB_HostRequest;

uint8_t USB_Host_SendControlRequest(uint8_t* DataBuffer)
{
	bool     SOFGenEnabled  = USB_HOST_SOFGeneration_IsEnabled();
	uint8_t  ReturnStatus   = HOST_SENDCONTROL_Sucessful;
	uint8_t* HeaderByte     = (uint8_t*)&USB_HostRequest;
	uint16_t DataLen        = USB_HostRequest.DataLength;

	USB_HOST_SOFGeneration_Enable();
	
	if ((ReturnStatus = USB_Host_WaitMS(1)) != HOST_WAITERROR_Sucessful)
	  return ReturnStatus;

	Pipe_ClearErrorFlags();

	Pipe_SelectPipe(PIPE_CONTROLPIPE);
	Pipe_SetToken(PIPE_TOKEN_SETUP);
	Pipe_ClearSetupSent();

	Pipe_Unfreeze();

	for (uint8_t i = 0; i < sizeof(USB_Host_Request_Header_t); i++)
	  Pipe_Write_Byte(*(HeaderByte++));

	Pipe_Setup_Out_Clear();
	
	if ((ReturnStatus = USB_Host_Wait_For_Setup_IOS(Wait_For_Setup_Sent)))
	  goto End_Of_Control_Send;

	Pipe_ClearSetupSent();
	Pipe_Freeze();

	if ((ReturnStatus = USB_Host_WaitMS(1)) != HOST_WAITERROR_Sucessful)
	  goto End_Of_Control_Send;

	if ((USB_HostRequest.RequestType & CONTROL_REQTYPE_DIRECTION) == REQDIR_DEVICETOHOST)
	{
		Pipe_SetInfiniteINRequests();
		Pipe_SetToken(PIPE_TOKEN_IN);
		
		while ((DataBuffer != NULL) && DataLen)
		{
			Pipe_Unfreeze();
		
			if ((ReturnStatus = USB_Host_Wait_For_Setup_IOS(Wait_For_In_Received)))
			  goto End_Of_Control_Send;
						
			if (!(Pipe_BytesInPipe()))
			  DataLen = 0;
			
			while (Pipe_BytesInPipe() && DataLen--)
			  *(DataBuffer++) = Pipe_Read_Byte();
		
			Pipe_Freeze();
			Pipe_Setup_In_Clear();
		}

		Pipe_SetToken(PIPE_TOKEN_OUT);
		Pipe_Unfreeze();
		
		if ((ReturnStatus = USB_Host_Wait_For_Setup_IOS(Wait_For_Out_Ready)))
		  goto End_Of_Control_Send;

		Pipe_Setup_Out_Clear();
	}
	else
	{
		Pipe_SetToken(PIPE_TOKEN_OUT);
		Pipe_Unfreeze();	

		while ((DataBuffer != NULL) && DataLen)
		{
			if (DataLen <= USB_ControlPipeSize)
			{
				while (DataLen--)
				  Pipe_Write_Byte(*(DataBuffer++));
			}
			else
			{
				for (uint16_t PipeByte = 0; PipeByte < USB_ControlPipeSize; PipeByte++)
				  Pipe_Write_Byte(*(DataBuffer++));

				DataLen -= USB_ControlPipeSize;			
			}
			
			if ((ReturnStatus = USB_Host_Wait_For_Setup_IOS(Wait_For_Out_Ready)))
			  goto End_Of_Control_Send;

			Pipe_Setup_Out_Clear();
		}
		
		Pipe_Freeze();
		Pipe_SetToken(PIPE_TOKEN_IN);
		Pipe_Unfreeze();

		if ((ReturnStatus = USB_Host_Wait_For_Setup_IOS(Wait_For_In_Received)))
		  goto End_Of_Control_Send;

		Pipe_Freeze();
		Pipe_Setup_In_Clear();
	}

End_Of_Control_Send:
	Pipe_Freeze();
	
	if (!(SOFGenEnabled))
	  USB_HOST_SOFGeneration_Disable();

	Pipe_ResetPipe(PIPE_CONTROLPIPE);

	return ReturnStatus;
}

static uint8_t USB_Host_Wait_For_Setup_IOS(uint8_t WaitType)
{
	uint8_t  ReturnStatus   = HOST_SENDCONTROL_Sucessful;
	uint16_t TimeoutCounter = USB_HOST_TIMEOUT_MS;

	while (!(((WaitType == Wait_For_Setup_Sent)  && Pipe_IsSetupSent())         ||
	         ((WaitType == Wait_For_In_Received) && Pipe_Setup_In_IsReceived()) ||
	         ((WaitType == Wait_For_Out_Ready)   && Pipe_Setup_Out_IsReady())))
	{
		if ((ReturnStatus = USB_Host_WaitMS(1)) != HOST_WAITERROR_Sucessful)
		  return ReturnStatus;
			
		if (!(TimeoutCounter--))
		{
			ReturnStatus = HOST_SENDCONTROL_SoftwareTimeOut;
			return ReturnStatus;
		}
	}

	return ReturnStatus;
}

#endif
