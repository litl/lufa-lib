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

#include "BluetoothHCICommands.h"

static   Bluetooth_HCICommand_Header_t HCICommandHeader;
static   Bluetooth_HCIEvent_Header_t   HCIEventHeader;

         uint8_t                       Bluetooth_HCIProcessingState;
static   uint8_t                       Bluetooth_NextHCIProcessingState;

static   uint8_t                       Bluetooth_RejectedDeviceAddress[6];

static uint8_t Bluetooth_SendHCICommand(void* Parameters, uint8_t ParamLength)
{
	uint8_t CommandBuffer[sizeof(HCICommandHeader) + HCICommandHeader.ParameterLength];

	USB_HostRequest = (USB_Host_Request_Header_t)
		{
			bmRequestType: (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_DEVICE),
			bRequest:      0,
			wValue:        0,
			wIndex:        0,
			wLength:       sizeof(CommandBuffer)
		};
		
	memset(CommandBuffer, 0x00, sizeof(CommandBuffer));
	memcpy(CommandBuffer, &HCICommandHeader, sizeof(HCICommandHeader));
	
	if (ParamLength)
	  memcpy(&CommandBuffer[sizeof(HCICommandHeader)], Parameters, ParamLength);	

	return USB_Host_SendControlRequest(CommandBuffer);
}

static bool Bluetooth_GetNextHCIEventHeader(void)
{
	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
	Pipe_Unfreeze();
	
	if (!(Pipe_ReadWriteAllowed()))
	  return false;
	  
	Pipe_Read_Stream_LE(&HCIEventHeader, sizeof(HCIEventHeader));
	  
	return true;
}

static void Bluetooth_DiscardRemainingHCIEventParameters(void)
{
	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
	Pipe_Discard_Stream(HCIEventHeader.ParameterLength);
	Pipe_ClearCurrentBank();
}

void Bluetooth_ProcessHCICommands(void)
{
	uint8_t ErrorCode;

	switch (Bluetooth_HCIProcessingState)
	{
		case Bluetooth_Init:
			Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
			Pipe_SetInfiniteINRequests();
			
			Bluetooth_Connection.IsConnected = false;

			Bluetooth_HCIProcessingState = Bluetooth_Init_Reset; 
			break;
		case Bluetooth_Init_Reset:
			HCICommandHeader = (Bluetooth_HCICommand_Header_t)
			{
				OpCode: {OGF: OGF_CTRLR_BASEBAND, OCF: OCF_CTRLR_BASEBAND_RESET},
				ParameterLength: 0,
			};
			
			printf("(HCI) Enter State: Bluetooth_Init_Reset\r\n");

			ErrorCode = Bluetooth_SendHCICommand(NULL, 0);
			Bluetooth_HCIProcessingState     = Bluetooth_ProcessEvents;
			Bluetooth_NextHCIProcessingState = Bluetooth_Init_SetLocalName;
			break;
		case Bluetooth_Init_SetLocalName:
			HCICommandHeader = (Bluetooth_HCICommand_Header_t)
				{
					OpCode: {OGF: OGF_CTRLR_BASEBAND, OCF: OCF_CTRLR_BASEBAND_WRITE_LOCAL_NAME},
					ParameterLength: 248,
				};

			printf("(HCI) Enter State: Bluetooth_Init_SetLocalName\r\n");

			ErrorCode                = Bluetooth_SendHCICommand(Bluetooth_DeviceConfiguration.Name, strlen(Bluetooth_DeviceConfiguration.Name));
			Bluetooth_HCIProcessingState     = Bluetooth_ProcessEvents;
			Bluetooth_NextHCIProcessingState = Bluetooth_Init_SetDeviceClass;
			break;
		case Bluetooth_Init_SetDeviceClass:
			HCICommandHeader = (Bluetooth_HCICommand_Header_t)
				{
					OpCode: {OGF: OGF_CTRLR_BASEBAND, OCF: OCF_CTRLR_BASEBAND_WRITE_CLASS_OF_DEVICE},
					ParameterLength: 3,
				};

			printf("(HCI) Enter State: Bluetooth_Init_SetDeviceClass\r\n");

			ErrorCode = Bluetooth_SendHCICommand(&Bluetooth_DeviceConfiguration.Class, 3);
			Bluetooth_HCIProcessingState     = Bluetooth_ProcessEvents;
			Bluetooth_NextHCIProcessingState = Bluetooth_Init_WriteScanEnable;	
			break;
		case Bluetooth_Init_WriteScanEnable:
			HCICommandHeader = (Bluetooth_HCICommand_Header_t)
			{
				OpCode: {OGF: OGF_CTRLR_BASEBAND, OCF: OCF_CTRLR_BASEBAND_WRITE_SCAN_ENABLE},
				ParameterLength: 1,
			};
			
			printf("(HCI) Enter State: Bluetooth_Init_WriteScanEnable\r\n");

			uint8_t Interval = InquiryAndPageScans;

			ErrorCode = Bluetooth_SendHCICommand(&Interval, 1);			
			Bluetooth_HCIProcessingState     = Bluetooth_ProcessEvents;
			Bluetooth_NextHCIProcessingState = Bluetooth_ProcessEvents;
			break;
		case Bluetooth_Conn_AcceptConnection:
			HCICommandHeader = (Bluetooth_HCICommand_Header_t)
				{
					OpCode: {OGF: OGF_LINK_CONTROL, OCF: OCF_LINK_CONTROL_ACCEPT_CONNECTION_REQUEST},
					ParameterLength: sizeof(Bluetooth_HCICommand_AcceptConnectionRequest_Params_t),
				};
			
			printf("(HCI) Enter State: Bluetooth_Conn_AcceptConnection\r\n");

			Bluetooth_HCICommand_AcceptConnectionRequest_Params_t AcceptConnectionParams;
							 
			memcpy(AcceptConnectionParams.RemoteAddress, Bluetooth_Connection.DeviceAddress,
			       sizeof(Bluetooth_Connection.DeviceAddress));
			AcceptConnectionParams.SlaveRole = 0x01;

			Bluetooth_SendHCICommand(&AcceptConnectionParams, sizeof(AcceptConnectionParams));
		
			Bluetooth_Connection.IsConnected = true;

			Bluetooth_HCIProcessingState     = Bluetooth_ProcessEvents;
			Bluetooth_NextHCIProcessingState = Bluetooth_ProcessEvents;
			break;
		case Bluetooth_Conn_RejectConnection:
			HCICommandHeader = (Bluetooth_HCICommand_Header_t)
				{
					OpCode: {OGF: OGF_LINK_CONTROL, OCF: OCF_LINK_CONTROL_ACCEPT_CONNECTION_REQUEST},
					ParameterLength: sizeof(Bluetooth_HCICommand_RejectConnectionRequest_Params_t),
				};
			
			printf("(HCI) Enter State: Bluetooth_Conn_RejectConnection\r\n");

			Bluetooth_HCICommand_RejectConnectionRequest_Params_t RejectConnectionParams;

			memcpy(RejectConnectionParams.RemoteAddress, Bluetooth_RejectedDeviceAddress,
			       sizeof(Bluetooth_RejectedDeviceAddress));
			RejectConnectionParams.Reason = ERROR_LIMITED_RESOURCES;

			Bluetooth_SendHCICommand(&AcceptConnectionParams, sizeof(AcceptConnectionParams));
		
			Bluetooth_HCIProcessingState     = Bluetooth_ProcessEvents;
			Bluetooth_NextHCIProcessingState = Bluetooth_ProcessEvents;
			break;
		case Bluetooth_ProcessEvents:
			if (Bluetooth_GetNextHCIEventHeader())
			{
				printf("(HCI) Event Code: 0x%02X\r\n", HCIEventHeader.EventCode);
			
				if (HCIEventHeader.EventCode == EVENT_COMMAND_STATUS)
				{
					Bluetooth_HCIEvent_CommandStatus_Header_t CommandStatusHeader;
					Pipe_Read_Stream_LE(&CommandStatusHeader, sizeof(CommandStatusHeader));
					
					printf("(HCI) >> Command status: 0x%02X\r\n", CommandStatusHeader.CommandStatus);
					
					if (CommandStatusHeader.CommandStatus)
					  Bluetooth_HCIProcessingState = Bluetooth_Init;

					HCIEventHeader.ParameterLength -= sizeof(CommandStatusHeader);
				}
				else if (HCIEventHeader.EventCode == EVENT_COMMAND_COMPLETE)
				{
					printf("(HCI) >> Command complete.\r\n");

					Bluetooth_HCIProcessingState = Bluetooth_NextHCIProcessingState;
					
					if (Bluetooth_HCIProcessingState == Bluetooth_ProcessEvents)
					  printf("(HCI) Enter State: Bluetooth_ProcessEvents\r\n");
				}
				else if (HCIEventHeader.EventCode == EVENT_CONNECTION_REQUEST)
				{
					Bluetooth_HCIEvent_ConnectionRequest_Header_t ConnectionRequestParams;
					
					Pipe_Read_Stream_LE(&ConnectionRequestParams, sizeof(ConnectionRequestParams));

					uint8_t* RemoteAddressOctets = (uint8_t*)&ConnectionRequestParams.RemoteAddress;
					printf_P(PSTR("(HCI) >> Connection Request from device %02X:%02X:%02X:%02X:%02X:%02X\r\n"), 
							 RemoteAddressOctets[5], RemoteAddressOctets[4], RemoteAddressOctets[3],
							 RemoteAddressOctets[2], RemoteAddressOctets[1], RemoteAddressOctets[0]);
					printf_P(PSTR("(HCI) -- Device Class: 0x%02X%04X\r\n"), ConnectionRequestParams.ClassOfDevice_Service,
																	   ConnectionRequestParams.ClassOfDevice_MajorMinor);
					printf_P(PSTR("(HCI) -- Link Type: 0x%02x\r\n"), ConnectionRequestParams.LinkType);
					printf_P(PSTR("(HCI) -- Connection %S\r\n"), (Bluetooth_Connection.IsConnected) ? PSTR("Rejected") : PSTR("Accepted"));
					
					if (Bluetooth_Connection.IsConnected)
					{
						memcpy(Bluetooth_RejectedDeviceAddress, ConnectionRequestParams.RemoteAddress,
						       sizeof(Bluetooth_RejectedDeviceAddress));
						
						Bluetooth_HCIProcessingState = Bluetooth_Conn_RejectConnection;
					}
					else
					{
						memcpy(Bluetooth_Connection.DeviceAddress, ConnectionRequestParams.RemoteAddress,
						       sizeof(Bluetooth_Connection.DeviceAddress));

						Bluetooth_HCIProcessingState = Bluetooth_Conn_AcceptConnection;
					}

					HCIEventHeader.ParameterLength -= sizeof(ConnectionRequestParams);
				}
				else if (HCIEventHeader.EventCode == EVENT_DISCONNECTION_COMPLETE)
				{
					printf("(HCI) >> Disconnection from device complete.\r\n");
					Bluetooth_Connection.IsConnected = false;					
				}
				
				Bluetooth_DiscardRemainingHCIEventParameters();
			}

			break;
	}
}
