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

#ifndef _BLUETOOTH_COMMANDS_H_
#define _BLUETOOTH_COMMANDS_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		#include <stdbool.h>

		#include <LUFA/Drivers/USB/USB.h>
		
		#include "BluetoothHost.h"
		#include "BluetoothClassCodes.h"
		#include "BluetoothCommandCodes.h"
		
	/* Macros: */
		#define CHANNEL_SIGNALLING      0x0001
		#define CHANNEL_CONNECTIONLESS  0x0002
	
	/* Type Defines: */
		typedef struct
		{
			struct
			{
				int OCF : 10;
				int OGF : 6;
			} OpCode;

			uint8_t  ParameterLength;
			uint8_t  Parameters[];
		} Bluetooth_HCICommand_Header_t;

		typedef struct
		{
			uint8_t  EventCode;
			uint8_t  ParameterLength;
		} Bluetooth_HCIEvent_Header_t;
		
		typedef struct
		{
			uint16_t ConnectionHandle;
			uint16_t DataLength;
		} Bluetooth_ACL_Header_t;

		typedef struct
		{
			uint16_t PayloadLength;
			uint16_t ChannelID;
		} Bluetooth_DataPacket_Header_t;
		
		typedef struct
		{
			uint8_t Code;
			uint8_t Identifier;
		} Bluetooth_SignalCommand_Header_t;

		typedef struct
		{
			uint8_t Octets[6];
		} Bluetooth_Device_Address_t;	
		
		typedef struct
		{
			Bluetooth_Device_Address_t RemoteAddress;
			uint8_t                    ClassOfDevice_Service;
			uint16_t                   ClassOfDevice_MajorMinor;
			uint8_t                    LinkType;
		} Bluetooth_EventParams_ConnectionRequest_t;
		
		typedef struct
		{
			uint8_t                    ConnectionStatus;
			uint16_t                   ConnectionHandle;
			Bluetooth_Device_Address_t RemoteAddress;
			uint8_t                    LinkType;
			uint8_t                    EncryptionEnabled;
		} Bluetooth_EventParams_ConnectionComplete_t;

		typedef struct
		{
			uint8_t                    ConnectionStatus;
			uint16_t                   ConnectionHandle;
			uint8_t                    Reason;
		} Bluetooth_EventParams_DisconnectionComplete_t;
	
	/* Function Prototypes: */
	#if defined(INCLUDE_FROM_BLUETOOTH_COMMANDS_C)
		static uint8_t Bluetooth_SendHCICommand(void* Parameters);
	#endif

	void Bluetooth_Command_ResetBaseBand(void);
	void Bluetooth_Command_SetLocalName(char* NewLocalName);
	void Bluetooth_Command_GetRemoteName(Bluetooth_Device_Address_t* RemoteAddress, char* RemoteNameBuff);
	void Bluetooth_Command_WriteScanEnable(uint8_t Interval);
	void Bluetooth_Command_AcceptConnectionRequest(Bluetooth_Device_Address_t* RemoteAddress, bool SlaveRole);
	void Bluetooth_Command_WriteClassOfDevice(uint32_t DeviceClass);
	
#endif
