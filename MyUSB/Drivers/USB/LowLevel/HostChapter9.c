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

void USB_Host_SendControlRequest(const void* Data)
{
	uint8_t* HeaderByte = (uint8_t*)&USB_HostRequest;

	if (USB_Host_WaitMS(1) != HOST_WAITERROR_Sucessful)
		return;

	Pipe_SelectPipe(0);
	Pipe_SetToken(PIPE_TOKEN_SETUP);
	Pipe_ClearSetupSent();

	Pipe_Unfreeze();

	for (uint8_t i = 0; i < sizeof(USB_Host_Request_Header_t); i++)
	  USB_Host_Write_Byte(*(HeaderByte++));

	Pipe_SendPipeData();
	
	while(!(Pipe_IsSetupSent()))
	{
		if (USB_Host_WaitMS(1) != HOST_WAITERROR_Sucessful)
		{
			Pipe_Freeze();
			return;
		}
	}

	// DEBUG: \/\/\/
		
	if ((USB_HostRequest.RequestType & CONTROL_REQTYPE_DIRECTION) == REQDIR_DEVICETOHOST)
	{
		Pipe_SetToken(PIPE_TOKEN_OUT);
		Pipe_Unfreeze();

	}
	else
	{
		Pipe_SetInfiniteINRequests();
		Pipe_SetToken(PIPE_TOKEN_IN);
		Pipe_Unfreeze();
		
		puts_P(PSTR("Waiting for IN data."));
		while (!(Pipe_IsSetupInRecieved()) && !(Pipe_IsSetupStalled()));
		
		if (Pipe_IsSetupStalled())
		{
			Pipe_ClearSetupStalled();
			puts_P(PSTR("Transaction Stalled."));
			for (;;);
		}
		
		puts_P(PSTR("Response Data:"));
		while (Pipe_BytesInPipe())
			printf_P(PSTR("%d, "), USB_Host_Read_Byte());

		for (;;);
	}

	Pipe_Freeze();
};
#endif
