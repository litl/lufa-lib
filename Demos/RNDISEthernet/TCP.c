/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/* Basic TCP stack for TCP connections. While up to MAX_PORT_CONNECTIONS number of connections
   to seperate ports may be established from one machine, only one machine may connect at the
   one time.
*/

#define   INCLUDE_FROM_TCP_C
#include "TCP.h"

/* Global Variables: */
TCP_ConnectionState_t ConnectionStateTable[MAX_PORT_CONNECTIONS];


void TCP_Init(void)
{
	/* Initialize the connection table with all CLOSED entries */
	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_PORT_CONNECTIONS; CSTableEntry++)
	  ConnectionStateTable[CSTableEntry].State = TCP_Connection_Closed;
}

uint8_t TCP_GetConnectionState(uint16_t Port)
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_PORT_CONNECTIONS; CSTableEntry++)
	{
		/* Find port entry in the table */
		if (ConnectionStateTable[CSTableEntry].Port == Port)
		  return ConnectionStateTable[CSTableEntry].State;
	}
	
	return TCP_Connection_Closed;
}

void TCP_SetConnectionState(uint16_t Port, uint8_t State)
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_PORT_CONNECTIONS; CSTableEntry++)
	{
		/* Find port entry in the table */
		if (ConnectionStateTable[CSTableEntry].Port == Port)
		{
			ConnectionStateTable[CSTableEntry].State = State;
			return;
		}
	}
}

uint16_t TCP_ProcessTCPPacket(void* InDataStart, void* OutDataStart)
{
	Ethernet_TCP_Header_t* TCPHeaderIN = (Ethernet_TCP_Header_t*)InDataStart;
	Ethernet_TCP_Header_t* TCPHeaderOUT = (Ethernet_TCP_Header_t*)OutDataStart;

	if (TCP_IsListening(TCPHeaderIN->DestinationPort))
	{
		switch (TCP_GetConnectionState(TCPHeaderIN->DestinationPort))
		{
			case TCP_Connection_Listen:
				if (TCPHeaderIN->Flags == TCP_FLAG_SYN)
				{
					// Send back SYN+ACK header
					TCPHeaderOUT->SourcePort      = TCPHeaderIN->DestinationPort;
					TCPHeaderOUT->DestinationPort = TCPHeaderIN->SourcePort;
				
					TCP_SetConnectionState(TCPHeaderIN->DestinationPort, TCP_Connection_SYNReceived);
				}
				
				break;
			case TCP_Connection_SYNReceived:
				if (TCPHeaderIN->Flags == TCP_FLAG_ACK)
				  TCP_SetConnectionState(TCPHeaderIN->DestinationPort, TCP_Connection_Established);
				
				break;
			case TCP_Connection_Established:
				if (TCPHeaderIN->Flags == TCP_FLAG_FIN)
				{
					// Send back FIN+ACK header

					TCP_SetConnectionState(TCPHeaderIN->DestinationPort, TCP_Connection_CloseWait);
				}
				
				break;
			case TCP_Connection_CloseWait:
				if (TCPHeaderIN->Flags == TCP_FLAG_ACK)
				  TCP_SetConnectionState(TCPHeaderIN->DestinationPort, TCP_Connection_Listen);
				
				break;			
		}
	}

	return NO_RESPONSE;
}
