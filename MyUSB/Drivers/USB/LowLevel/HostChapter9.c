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

	Pipe_SelectPipe(0);
	Pipe_SetToken(PIPE_TOKEN_SETUP);
	Pipe_ClearSetupReady();

	Pipe_Unfreeze();

	for (uint8_t i = 0; i < sizeof(USB_Host_Request_Header_t); i++)
	  USB_Host_Write_Byte(*(HeaderByte++));

	Pipe_SendPipeData();
	
	// DEBUG:
	printf("Waiting for response.\r\n");
	while (!(Pipe_BytesInPipe()));

	printf("Pipe Data:");
	while(Pipe_BytesInPipe())
		printf("%d, ", USB_Host_Read_Byte());
};
