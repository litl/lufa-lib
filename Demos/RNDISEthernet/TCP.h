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
		
		#include "Ethernet.h"
		
	/* Macros: */
		#define MAX_PORT_CONNECTIONS            1
		
		#define TCP_PORT_HTTP                   80
		
		#define TCP_FLAG_CWR                    (1 << 7)
		#define TCP_FLAG_ECE                    (1 << 6)
		#define TCP_FLAG_URG                    (1 << 5)
		#define TCP_FLAG_ACK                    (1 << 4)
		#define TCP_FLAG_PSH                    (1 << 3)
		#define TCP_FLAG_RST                    (1 << 2)
		#define TCP_FLAG_SYN                    (1 << 1)
		#define TCP_FLAG_FIN                    (1 << 0)
		
		#define TCP_StartListening(Port)        TCP_SetConnectionState(Port, TCP_Connection_Listen)
		#define TCP_StopListening(Port)         TCP_SetConnectionState(Port, TCP_Connection_Closed)
		#define TCP_IsListening(Port)           (TCP_GetConnectionState(Port) != TCP_Connection_Closed)

	/* Enums: */
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
			uint16_t Port;
			uint8_t  State;
		} TCP_ConnectionState_t;

	/* Function Prototypes: */
		void     TCP_Init(void);
		uint8_t  TCP_GetConnectionState(uint16_t Port);
		void     TCP_SetConnectionState(uint16_t Port, uint8_t State);
		uint16_t TCP_ProcessTCPPacket(void* InDataStart, void* OutDataStart);

#endif
