/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "Host.h"

USB_Host_Request_Header_t USB_HostRequest;

void USB_Host_SendControlRequest(const void* Data)
{
	uint8_t* HeaderByte = (uint8_t*)&USB_HostRequest;

	if (USB_Host_WaitOneFrame() == false)
		return;

	Pipe_SelectPipe(0);
	Pipe_SetToken(PIPE_TOKEN_SETUP);
	Pipe_ClearSetupRecieved();

	Pipe_Unfreeze();

	for (uint8_t i = 0; i < sizeof(USB_Host_Request_Header_t); i++)
	  USB_Host_Write_Byte(*(HeaderByte++));

	Pipe_SendPipeData();
	
	while(!(Pipe_IsSetupRecieved()))
	{
		if (USB_Host_WaitOneFrame() == false)
		{
			Pipe_Freeze();
			return;
		}
	}

	puts_P(PSTR("Setup token recieved.\r\n"));
	// TODO: Get/Set data
		
	if ((USB_HostRequest.RequestType & CONTROL_REQTYPE_DIRECTION) == REQDIR_DEVICETOHOST)
	{
		Pipe_SetToken(PIPE_TOKEN_OUT);

	}
	else
	{
		Pipe_SetToken(PIPE_TOKEN_IN);
		Pipe_SetInfiniteINRequests();
		
		puts_P(PSTR("Waiting for IN data."));
		while (!(Pipe_BytesInPipe()));
		
		puts_P(PSTR("Response Data:"));
		while (Pipe_BytesInPipe())
			printf_P(PSTR("%d"), USB_Host_Read_Byte());

		for (;;);
	}
	
	Pipe_Freeze();
};
