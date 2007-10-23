/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#include "Host.h"

USB_Host_Request_Header HostRequest;

void USB_Host_SendControlRequest(const void* Data)
{
	uint8_t* HeaderByte = (uint8_t*)&HostRequest;

	Pipe_SelectPipe(0);
	Pipe_Unfreeze();

	for (uint8_t i = 0; i < sizeof(USB_Host_Request_Header); i++)
	  USB_Host_Write_Byte(*(HeaderByte++));

	Pipe_SendPipeData();
};
