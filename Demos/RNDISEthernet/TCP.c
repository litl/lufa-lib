/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/* Simple TCP stack to handle TCP connections. Note that due to the underlying network structure, only ONE
   host can be serviced at a time despite the TCP code supporting multiple hosts. This is because the system
   uses a Packet In/Packet Out approach to all packets, and multiple hosts would require a more complicated
   system to allow for ARP requests on hosts to map back from IP to MAC before a TCP packet is returned.
*/

#define   INCLUDE_FROM_TCP_C
#include "TCP.h"

/* Global Variables: */
TCP_PortState_t       PortStateTable[MAX_OPEN_PORTS];
TCP_ConnectionState_t ConnectionStateTable[MAX_CONNECTIONS];


void TCP_Init(void)
{
	/* Initialize the port state table with all CLOSED entries */
	for (uint8_t PTableEntry = 0; PTableEntry < MAX_CONNECTIONS; PTableEntry++)
	  PortStateTable[PTableEntry].State = TCP_Port_Closed;

	/* Initialize the connection table with all CLOSED entries */
	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_CONNECTIONS; CSTableEntry++)
	  ConnectionStateTable[CSTableEntry].State = TCP_Connection_Closed;
}

bool TCP_SetPortState(uint16_t Port, uint8_t State)
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t PTableEntry = 0; PTableEntry < MAX_CONNECTIONS; PTableEntry++)
	{
		/* Find existing entry for the port in the table, update it if found */
		if (PortStateTable[PTableEntry].Port == Port)
		{
			PortStateTable[PTableEntry].State = State;
			return true;
		}
	}

	/* Check if trying to open the port -- if so we need to find an unused (closed) entry and replace it */
	if (State == TCP_Port_Open)
	{
		for (uint8_t PTableEntry = 0; PTableEntry < MAX_CONNECTIONS; PTableEntry++)
		{
			/* Find a closed port entry in the table, change it to the given port and state */
			if (PortStateTable[PTableEntry].State == TCP_Port_Closed)
			{			
				PortStateTable[PTableEntry].Port  = Port;
				PortStateTable[PTableEntry].State = State;
				return true;
			}
		}
		
		/* Port not in table and no room to add it, return failure */
		return false;
	}
	else
	{
		/* Port not in table but trying to close it, so operation successful */
		return true;
	}
}

uint8_t TCP_GetPortState(uint16_t Port)
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t PTableEntry = 0; PTableEntry < MAX_CONNECTIONS; PTableEntry++)
	{
		/* Find existing entry for the port in the table, return the port status if found */
		if (PortStateTable[PTableEntry].Port == Port)
		  return PortStateTable[PTableEntry].State;
	}
	
	/* Port not in table, assume closed */
	return TCP_Port_Closed;
}
		
uint8_t TCP_GetConnectionState(uint16_t Port, IP_Address_t RemoteAddress)
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_CONNECTIONS; CSTableEntry++)
	{
		/* Find port entry in the table */
		if ((ConnectionStateTable[CSTableEntry].Port == Port) &&
		     IP_COMPARE(&ConnectionStateTable[CSTableEntry].RemoteAddress, &RemoteAddress))
		{
			return ConnectionStateTable[CSTableEntry].State;
		}
	}
	
	return TCP_Connection_Closed;
}

bool TCP_SetConnectionState(uint16_t Port, IP_Address_t RemoteAddress, uint8_t State)
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_CONNECTIONS; CSTableEntry++)
	{
		/* Find port entry in the table */
		if ((ConnectionStateTable[CSTableEntry].Port == Port) &&
		     IP_COMPARE(&ConnectionStateTable[CSTableEntry].RemoteAddress, &RemoteAddress))
		{
			ConnectionStateTable[CSTableEntry].State = State;
			return true;
		}
	}
	
	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_CONNECTIONS; CSTableEntry++)
	{
		/* Find empty entry in the table */
		if (ConnectionStateTable[CSTableEntry].State == TCP_Connection_Closed)
		{
			ConnectionStateTable[CSTableEntry].Port          = Port;
			ConnectionStateTable[CSTableEntry].RemoteAddress = RemoteAddress;
			ConnectionStateTable[CSTableEntry].State         = State;
			return true;
		}
	}
	
	return false;
}

uint16_t TCP_ProcessTCPPacket(void* IPHeaderInStart, void* TCPHeaderInStart, void* TCPHeaderOutStart)
{
	Ethernet_IP_Header_t*  IPHeaderIN   = (Ethernet_IP_Header_t*)IPHeaderInStart;
	Ethernet_TCP_Header_t* TCPHeaderIN  = (Ethernet_TCP_Header_t*)TCPHeaderInStart;
	Ethernet_TCP_Header_t* TCPHeaderOUT = (Ethernet_TCP_Header_t*)TCPHeaderOutStart;
	
	bool PacketResponse = false;
	
	/* Check if the destination port is open and allows incomming connections */
	if (TCP_GetPortState(TCPHeaderIN->DestinationPort) == TCP_Port_Open)
	{
		/* Check if the packet is a reset packet */
		if (TCPHeaderIN->Flags == TCP_FLAG_RST)
		{
			/* Reset the connection to its default closed state, RST should return no reply */
			TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
					               TCP_Connection_Closed);
		}
		else
		{
			/* Process the incomming TCP packet based on the current connection state for the sender and port */
			switch (TCP_GetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress))
			{
				case TCP_Connection_Closed:
					if (TCPHeaderIN->Flags == TCP_FLAG_SYN)
					{
						TCPHeaderOUT->Flags = (TCP_FLAG_SYN | TCP_FLAG_ACK);				
						PacketResponse      = true;
									
						TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
											   TCP_Connection_SYNReceived);
											   
						printf("CLOSED->SYNRECEIVED\r\n");
					}
					else
					{
						printf("CLOSED->SELF\r\n");				
					}
					
					break;
				case TCP_Connection_SYNReceived:
					if (TCPHeaderIN->Flags == TCP_FLAG_ACK)
					{
						TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
											   TCP_Connection_Established);

						printf("SYNRECEIVED->ESTABLISHED\r\n");
					}
					else
					{
						printf("SYNRECEIVED->SELF\r\n");				
					}
					
					break;
				case TCP_Connection_Established:
					if (TCPHeaderIN->Flags == TCP_FLAG_FIN)
					{
						TCPHeaderOUT->Flags = (TCP_FLAG_FIN | TCP_FLAG_ACK);				
						PacketResponse      = true;

						TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
											   TCP_Connection_CloseWait);

						printf("ESTABLISHED->CLOSEWAIT\r\n");
					}
					else if (TCPHeaderIN->Flags == TCP_FLAG_ACK)
					{
						printf("ESTABLISHED->SELF\r\n");

						
						uint8_t* TCPData = &((uint8_t*)TCPHeaderInStart)[(TCPHeaderIN->DataOffset * sizeof(uint32_t))];
						
						FrameIN.FrameLength -= (TCPHeaderIN->DataOffset * sizeof(uint32_t));

						while (FrameIN.FrameLength)
						{
							printf("%c ", *(TCPData++));
							FrameIN.FrameLength--;
						}
						
						printf("\r\n");
					}
					
					break;
				case TCP_Connection_CloseWait:
					if (TCPHeaderIN->Flags == TCP_FLAG_ACK)
					{
						TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
											   TCP_Connection_Listen);

						printf("CLOSEWAIT->LISTEN\r\n");
					}
					else
					{
						printf("CLOSEWAIT->SELF\r\n");			
					}
					
					break;			
			}
		}
	}
	else
	{
		/* Port is not open, indicate via a RST/ACK response to the sender */
		TCPHeaderOUT->Flags = (TCP_FLAG_RST | TCP_FLAG_ACK);				
		PacketResponse      = true;
	}
	
	if (PacketResponse)
	{
		TCPHeaderOUT->SourcePort           = TCPHeaderIN->DestinationPort;
		TCPHeaderOUT->DestinationPort      = TCPHeaderIN->SourcePort;
		TCPHeaderOUT->SequenceNumber       = 0;
		TCPHeaderOUT->AcknowledgmentNumber = SwapEndian_32((SwapEndian_32(TCPHeaderIN->SequenceNumber) + 1));
		TCPHeaderOUT->DataOffset           = (sizeof(Ethernet_TCP_Header_t) / sizeof(uint32_t));
		TCPHeaderOUT->WindowSize           = SwapEndian_16(128);
		TCPHeaderOUT->UrgentPointer        = 0;
		TCPHeaderOUT->Checksum             = 0;
		
		TCPHeaderOUT->Checksum             = TCP_Checksum16(IPHeaderInStart, TCPHeaderOUT,
															sizeof(Ethernet_TCP_Header_t));					

		return sizeof(Ethernet_TCP_Header_t);	
	}

	return NO_RESPONSE;
}

static uint16_t TCP_Checksum16(void* IPHeaderInStart, void* TCPHeaderOutStart, uint16_t TCPOutSize)
{
	Ethernet_IP_Header_t*      IPHeaderIn   = (Ethernet_IP_Header_t*)IPHeaderInStart;
	TCP_Checksum_PsudoHeader_t PsudoIPHeader;

	union
	{
		uint32_t  DWord;
		uint16_t  Words[2];
	} Checksum = {0};
	
	PsudoIPHeader.SourceAddress      = IPHeaderIn->DestinationAddress;
	PsudoIPHeader.DestinationAddress = IPHeaderIn->SourceAddress;
	PsudoIPHeader.Protocol           = PROTOCOL_TCP;
	PsudoIPHeader.TCPLength          = SwapEndian_16(TCPOutSize);

	/* TCP/IP checksums are the addition of the one's compliment of each word, complimented */
	
	for (uint8_t CurrWord = 0; CurrWord < (sizeof(TCP_Checksum_PsudoHeader_t) >> 1); CurrWord++)
	  Checksum.DWord += ((uint16_t*)&PsudoIPHeader)[CurrWord];

	for (uint8_t CurrWord = 0; CurrWord < (TCPOutSize >> 1); CurrWord++)
	  Checksum.DWord += ((uint16_t*)TCPHeaderOutStart)[CurrWord];
	
	if (TCPOutSize & 0x01)
	  Checksum.DWord += (((uint16_t*)TCPHeaderOutStart)[TCPOutSize >> 1] & 0xFF00);
	
	return ~(Checksum.Words[0] + Checksum.Words[1]);
}
