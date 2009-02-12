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

#define  INCLUDE_FROM_BLUETOOTH_ACLPACKETS_C
#include "BluetoothACLPackets.h"

void Bluetooth_ProcessACLPackets(void)
{
	Bluetooth_ACL_Header_t ACLPacketHeader;

	Pipe_SelectPipe(BLUETOOTH_DATA_IN_PIPE);
	Pipe_SetToken(PIPE_TOKEN_IN);
	Pipe_Unfreeze();
	
	if (!(Pipe_ReadWriteAllowed()))
	{
		Pipe_Freeze();
		return;
	}
	  
	Pipe_Read_Stream_LE(&ACLPacketHeader, sizeof(ACLPacketHeader));
		
	Bluetooth_DataPacket_Header_t DataHeader;
	Pipe_Read_Stream_LE(&DataHeader, sizeof(DataHeader));
	DataHeader.PayloadLength -= sizeof(DataHeader);

	puts_P(PSTR("(ACL) Packet Received\r\n"));
	printf_P(PSTR("(ACL) -- Connection Handle: 0x%04X\r\n"), ACLPacketHeader.ConnectionHandle);
	printf_P(PSTR("(ACL) -- Data Length: 0x%04X\r\n"), ACLPacketHeader.DataLength);
	printf_P(PSTR("(ACL) -- Destination Channel: 0x%04X\r\n"), DataHeader.DestinationChannel);
	printf_P(PSTR("(ACL) -- Payload Length: 0x%04X\r\n"), DataHeader.PayloadLength);

	if (DataHeader.DestinationChannel == BLUETOOTH_CHANNEL_SIGNALING)
	{
		Bluetooth_SignalCommand_Header_t SignalCommandHeader;
		Pipe_Read_Stream_LE(&SignalCommandHeader, sizeof(SignalCommandHeader));
		DataHeader.PayloadLength -= sizeof(SignalCommandHeader);

		if (SignalCommandHeader.Code == BLUETOOTH_SIGNAL_CONNECTION_REQUEST)
		{
			Bluetooth_SignalCommand_ConnectionRequest_t ConnectionRequest;
			
			Pipe_Read_Stream_LE(&ConnectionRequest, sizeof(ConnectionRequest));
			Pipe_ClearCurrentBank();
			Pipe_Freeze();

			puts_P(PSTR("(ACL) >> L2CAP Connection Request\r\n"));
			printf_P(PSTR("(ACL) -- PSM: 0x%04X\r\n"), ConnectionRequest.PSM);
			printf_P(PSTR("(ACL) -- Source Channel: 0x%04X\r\n"), ConnectionRequest.SourceChannel);
			
			Pipe_SelectPipe(BLUETOOTH_DATA_OUT_PIPE);
			Pipe_SetToken(PIPE_TOKEN_OUT);
			Pipe_Unfreeze();
			
			Bluetooth_SignalCommand_ConnectionResponse_t ConnectionResponse;

			ACLPacketHeader.DataLength            = sizeof(DataHeader) + sizeof(SignalCommandHeader) + sizeof(ConnectionResponse);
			DataHeader.PayloadLength              = sizeof(SignalCommandHeader) + sizeof(ConnectionResponse);
			DataHeader.DestinationChannel         = BLUETOOTH_CHANNEL_SIGNALING;
			SignalCommandHeader.Code              = BLUETOOTH_SIGNAL_CONNECTION_RESPONSE;
			SignalCommandHeader.Length            = sizeof(ConnectionResponse);
			ConnectionResponse.DestinationChannel = ConnectionRequest.SourceChannel;
			ConnectionResponse.SourceChannel      = 0x0041; // FIXME - Allocated channel
			ConnectionResponse.Result             = 0;
			ConnectionResponse.Status             = 0;

			Pipe_Write_Stream_LE(&ACLPacketHeader, sizeof(ACLPacketHeader));
			Pipe_Write_Stream_LE(&DataHeader, sizeof(DataHeader));
			Pipe_Write_Stream_LE(&SignalCommandHeader, sizeof(SignalCommandHeader));
			Pipe_Write_Stream_LE(&ConnectionResponse, sizeof(ConnectionResponse));
			
			Pipe_ClearCurrentBank();		
			Pipe_Freeze();
			
			puts_P(PSTR("(ACL) Packet Sent\r\n"));
			printf_P(PSTR("(ACL) -- Connection Handle: 0x%04X\r\n"), ACLPacketHeader.ConnectionHandle);
			printf_P(PSTR("(ACL) -- Data Length: 0x%04X\r\n"), ACLPacketHeader.DataLength);
			printf_P(PSTR("(ACL) -- Destination Channel: 0x%04X\r\n"), DataHeader.DestinationChannel);
			printf_P(PSTR("(ACL) -- Payload Length: 0x%04X\r\n"), DataHeader.PayloadLength);			
			puts_P(PSTR("(ACL) >> L2CAP Connection Response\r\n"));
			printf_P(PSTR("(ACL) -- Source Channel: 0x%04X\r\n"), ConnectionResponse.SourceChannel);
			printf_P(PSTR("(ACL) -- Destination Channel: 0x%04X\r\n"), ConnectionResponse.DestinationChannel);
		}
		else
		{
			printf_P(PSTR("(ACL) >> Unknown Signalling Command 0x%02X\r\n"), SignalCommandHeader.Code);
				
			Pipe_Discard_Stream(ACLPacketHeader.DataLength);
			Pipe_ClearCurrentBank();		
			Pipe_Freeze();			
		}
	}
	else
	{
		uint8_t DataPayload[DataHeader.PayloadLength];
		Pipe_Read_Stream_LE(&DataPayload, sizeof(DataPayload));
		DataHeader.PayloadLength = 0;
	
		puts_P(PSTR("(ACL) -- Data Payload: "));
		for (uint16_t B = 0; B < sizeof(DataPayload); B++)
		  printf("0x%02X ", DataPayload[B]);
		puts_P(PSTR("\r\n"));

		Pipe_Discard_Stream(ACLPacketHeader.DataLength);
		Pipe_ClearCurrentBank();		
		Pipe_Freeze();
	}
}