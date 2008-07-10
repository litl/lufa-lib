/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _TCP_H_
#define _TCP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		#include <stdbool.h>
		
		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		
	/* Macros: */
		#define MAX_OPEN_PORTS                  2
		#define MAX_CONNECTIONS                 2
		
		#define TCP_PORT_HTTP                   SwapEndian_16(80)
		
		#define TCP_FLAG_CWR                    (1 << 7)
		#define TCP_FLAG_ECE                    (1 << 6)
		#define TCP_FLAG_URG                    (1 << 5)
		#define TCP_FLAG_ACK                    (1 << 4)
		#define TCP_FLAG_PSH                    (1 << 3)
		#define TCP_FLAG_RST                    (1 << 2)
		#define TCP_FLAG_SYN                    (1 << 1)
		#define TCP_FLAG_FIN                    (1 << 0)

	/* Enums: */
		enum TCP_PortStates_t
		{
			TCP_Port_Open              = 0,
			TCP_Port_Closed            = 1,
		};
	
		enum TCP_ConnectionStates_t
		{
			TCP_Connection_Listen      = 0,
			TCP_Connection_SYNSent     = 1,
			TCP_Connection_SYNReceived = 2,
			TCP_Connection_Established = 3,
			TCP_Connection_FINWait1    = 4,
			TCP_Connection_FINWait2    = 5,
			TCP_Connection_CloseWait   = 6,
			TCP_Connection_Closing     = 7,
			TCP_Connection_LastACK     = 8,
			TCP_Connection_TimeWait    = 9,
			TCP_Connection_Closed      = 10,			
		};
	
	/* Type Defines: */
		typedef struct
		{
			uint16_t     Port;
			uint8_t      State;
		} TCP_PortState_t;
	
		typedef struct
		{
			uint16_t     Port;
			IP_Address_t RemoteAddress;
			uint8_t      State;
		} TCP_ConnectionState_t;
		
		typedef struct
		{
			IP_Address_t  SourceAddress;
			IP_Address_t  DestinationAddress;
			
			uint8_t       Reserved;
			uint8_t       Protocol;
			uint16_t      TCPLength;
		} TCP_Checksum_PsudoHeader_t;

	/* Function Prototypes: */
		void     TCP_Init(void);
		bool     TCP_SetPortState(uint16_t Port, uint8_t State);
		uint8_t  TCP_GetPortState(uint16_t Port);
		uint8_t  TCP_GetConnectionState(uint16_t Port, IP_Address_t RemoteAddress);
		bool     TCP_SetConnectionState(uint16_t Port, IP_Address_t RemoteAddress, uint8_t State);
		uint16_t TCP_ProcessTCPPacket(void* IPHeaderInStart, void* TCPHeaderInStart, void* TCPHeaderOutStart);

		#if defined(INCLUDE_FROM_TCP_C)
			static uint16_t TCP_Checksum16(void* IPHeaderInStart, void* TCPHeaderOutStart, uint16_t TCPOutSize);
		#endif

#endif
