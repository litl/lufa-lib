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

#define INCLUDE_FROM_BLUETOOTH_COMMANDS_C
#include "BluetoothCommands.h"

Bluetooth_HCICommand_Header_t HCICommandHeader;

static uint8_t Bluetooth_SendHCICommand(void* Parameters)
{
	uint8_t CommandBuffer[sizeof(HCICommandHeader) + HCICommandHeader.ParameterLength];

	USB_HostRequest = (USB_Host_Request_Header_t)
		{
			bmRequestType: (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_DEVICE),
			bRequest:      0,
			wValue:        0,
			wIndex:        0,
			wLength:       sizeof(HCICommandHeader) + HCICommandHeader.ParameterLength
		};
		
	memcpy(&CommandBuffer[0], &HCICommandHeader, sizeof(HCICommandHeader));
	
	if (Parameters != NULL)
	  memcpy(&CommandBuffer[sizeof(HCICommandHeader)], Parameters, HCICommandHeader.ParameterLength);	
	
	return USB_Host_SendControlRequest(CommandBuffer);
}

void Bluetooth_Command_ResetBaseBand(void)
{
	Bluetooth_HCIEvent_Header_t EventInfo = {EventCode: 0x00};
			
	HCICommandHeader = (Bluetooth_HCICommand_Header_t)
		{
			OpCode: {OGF: OGF_CTRLR_BASEBAND, OCF: OCF_CTRLR_BASEBAND_RESET},
			ParameterLength: 0,
		};

	Bluetooth_SendHCICommand(NULL);

	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
	
	while (EventInfo.EventCode != EVENT_COMMAND_COMPLETE)
	{
		while (!(Pipe_ReadWriteAllowed()));	
		Pipe_Read_Stream_LE(&EventInfo, sizeof(EventInfo));
		Pipe_Discard_Stream(EventInfo.ParameterLength);
		Pipe_ClearCurrentBank();
	}
}

void Bluetooth_Command_SetLocalName(char* NewLocalName)
{
	Bluetooth_HCIEvent_Header_t EventInfo = {EventCode: 0x00};

	HCICommandHeader = (Bluetooth_HCICommand_Header_t)
		{
			OpCode: {OGF: OGF_CTRLR_BASEBAND, OCF: OCF_CTRLR_BASEBAND_WRITE_LOCAL_NAME},
			ParameterLength: 248,
		};
				
	char LocalName[248];
	memset(LocalName, 0x00, sizeof(LocalName));
	memcpy(LocalName, NewLocalName, strlen(NewLocalName));

	Bluetooth_SendHCICommand(&LocalName);

	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
					
	while (EventInfo.EventCode != EVENT_COMMAND_COMPLETE)
	{
		while (!(Pipe_ReadWriteAllowed()));	
		Pipe_Read_Stream_LE(&EventInfo, sizeof(EventInfo));
		Pipe_Discard_Stream(EventInfo.ParameterLength);
		Pipe_ClearCurrentBank();
	}
}

void Bluetooth_Command_GetRemoteName(Bluetooth_Device_Address_t* RemoteAddress, char* RemoteNameBuff)
{
	Bluetooth_HCIEvent_Header_t EventInfo = {EventCode: 0x00};

	HCICommandHeader = (Bluetooth_HCICommand_Header_t)
		{
			OpCode: {OGF: OGF_LINK_CONTROL, OCF: OCF_LINK_CONTROL_REMOTE_NAME_REQUEST},
			ParameterLength: sizeof(Bluetooth_Device_Address_t) + 4,
		};
	
	uint8_t RequestParams[sizeof(Bluetooth_Device_Address_t) + 4];
	memset(RequestParams, 0x00, sizeof(RequestParams));
	memcpy(RequestParams, RemoteAddress, sizeof(Bluetooth_Device_Address_t));

	Bluetooth_SendHCICommand(RequestParams);

	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);

	bool GotRemoteName = false;

	while (!(GotRemoteName))
	{
		while (!(Pipe_ReadWriteAllowed()));
						
		Pipe_Read_Stream_LE(&EventInfo, sizeof(EventInfo));
		
		if (EventInfo.EventCode == EVENT_REMOTE_NAME_REQUEST_COMPLETE)
		{
			Pipe_Discard_Stream(1 + sizeof(Bluetooth_Device_Address_t));
			Pipe_Read_Stream_LE(RemoteNameBuff, (EventInfo.ParameterLength - (1 + sizeof(Bluetooth_Device_Address_t))));
			GotRemoteName = true;
		}
		else
		{
			printf("Event Code: %d\r\n", EventInfo.EventCode);			
			printf("Error Code: %d\r\n", Pipe_Read_Byte());
			Pipe_Discard_Stream(EventInfo.ParameterLength - 1);
		}
		
		Pipe_ClearCurrentBank();
	}
}

void Bluetooth_Command_WriteScanEnable(uint8_t Interval)
{
	Bluetooth_HCIEvent_Header_t EventInfo = {EventCode: 0x00};

	HCICommandHeader = (Bluetooth_HCICommand_Header_t)
		{
			OpCode: {OGF: OGF_CTRLR_BASEBAND, OCF: OCF_CTRLR_BASEBAND_WRITE_SCAN_ENABLE},
			ParameterLength: 1,
		};
	
	Bluetooth_SendHCICommand(&Interval);

	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
					
	while (EventInfo.EventCode != EVENT_COMMAND_COMPLETE)
	{
		while (!(Pipe_ReadWriteAllowed()));	
		Pipe_Read_Stream_LE(&EventInfo, sizeof(EventInfo));
		Pipe_Discard_Stream(EventInfo.ParameterLength);
		Pipe_ClearCurrentBank();
	}
}

void Bluetooth_Command_AcceptConnectionRequest(Bluetooth_Device_Address_t* RemoteAddress, bool SlaveRole)
{
	Bluetooth_HCIEvent_Header_t EventInfo = {EventCode: 0x00};
	
	uint8_t Params[sizeof(Bluetooth_Device_Address_t) + 1];
	memcpy(Params, RemoteAddress, sizeof(Bluetooth_Device_Address_t));
	Params[sizeof(Bluetooth_Device_Address_t)] = SlaveRole;

	HCICommandHeader = (Bluetooth_HCICommand_Header_t)
		{
			OpCode: {OGF: OGF_LINK_CONTROL, OCF: OCF_LINK_CONTROL_ACCEPT_CONNECTION_REQUEST},
			ParameterLength: sizeof(Params),
		};
	
	Bluetooth_SendHCICommand(&Params);

	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
					
	while (EventInfo.EventCode != EVENT_COMMAND_STATUS)
	{
		while (!(Pipe_ReadWriteAllowed()));	
		Pipe_Read_Stream_LE(&EventInfo, sizeof(EventInfo));
		Pipe_Discard_Stream(EventInfo.ParameterLength);
		Pipe_ClearCurrentBank();
	}
}

void Bluetooth_Command_WriteClassOfDevice(uint32_t DeviceClass)
{
	Bluetooth_HCIEvent_Header_t EventInfo = {EventCode: 0x00};
	
	HCICommandHeader = (Bluetooth_HCICommand_Header_t)
		{
			OpCode: {OGF: OGF_CTRLR_BASEBAND, OCF: OCF_CTRLR_BASEBAND_WRITE_CLASS_OF_DEVICE},
			ParameterLength: 3,
		};
	
	Bluetooth_SendHCICommand(&DeviceClass);
	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
					
	while (EventInfo.EventCode != EVENT_COMMAND_COMPLETE)
	{
		while (!(Pipe_ReadWriteAllowed()));	
		Pipe_Read_Stream_LE(&EventInfo, sizeof(EventInfo));
		Pipe_Discard_Stream(EventInfo.ParameterLength);
		Pipe_ClearCurrentBank();
	}
}
