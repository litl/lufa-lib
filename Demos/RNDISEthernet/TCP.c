/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#define   INCLUDE_FROM_TCP_C
#include "TCP.h"

/* Global Variables: */
TCP_PortState_t        PortStateTable[MAX_OPEN_TCP_PORTS];
TCP_ConnectionState_t  ConnectionStateTable[MAX_TCP_CONNECTIONS];


TASK(TCP_Task)
{
	/* Task to hand off TCP packets to and from the listening applications. */
	  
	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
	{
		/* For each completely received packet, pass it along to the listening application */
		if ((ConnectionStateTable[CSTableEntry].Info.Buffer.Direction == TCP_PACKETDIR_IN) &&
		    (ConnectionStateTable[CSTableEntry].Info.Buffer.Ready))
		{
			printf("Packet ready for port %u.\r\n", SwapEndian_16(ConnectionStateTable[CSTableEntry].Port));
		
			/* Find the corresponding port entry in the port table */
			for (uint8_t PTableEntry = 0; PTableEntry < MAX_TCP_CONNECTIONS; PTableEntry++)
			{
				/* Run the application handler for the port on the received packet */
				if ((PortStateTable[PTableEntry].Port  == ConnectionStateTable[CSTableEntry].Port) && 
				    (PortStateTable[PTableEntry].State == TCP_Port_Open))
				{
					printf("Running app handler for port %u.\r\n", SwapEndian_16(PortStateTable[PTableEntry].Port));
					ConnectionStateTable[CSTableEntry].Info.Buffer.Direction = TCP_PACKETDIR_OUT;
					PortStateTable[PTableEntry].ApplicationHandler(&ConnectionStateTable[CSTableEntry].Info.Buffer);
					printf("Handler Complete.\r\n");
				}
			}
		}
	}
	
	/* Bail out early if there is already a frame waiting to be sent in the Ethernet OUT buffer */
	if (FrameOUT.FrameInBuffer)
	  return;
	
	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
	{
		/* For each completely received packet, pass it along to the listening application */
		if ((ConnectionStateTable[CSTableEntry].Info.Buffer.Direction == TCP_PACKETDIR_OUT) &&
		    (ConnectionStateTable[CSTableEntry].Info.Buffer.Ready))
		{
			Ethernet_Frame_Header_t* FrameOUTHeader = (Ethernet_Frame_Header_t*)&FrameOUT.FrameData;
			IP_Header_t*             IPHeaderOUT    = (IP_Header_t*)&FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t)];
			TCP_Header_t*            TCPHeaderOUT   = (TCP_Header_t*)&FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t) +
			                                                                             sizeof(IP_Header_t)];						
			void*                    TCPDataOUT     = &FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t) +
			                                                              sizeof(IP_Header_t) +
			                                                              sizeof(TCP_Header_t)];

			uint16_t PacketSize = ConnectionStateTable[CSTableEntry].Info.Buffer.Length;

			/* Fill out the TCP data */
			TCPHeaderOUT->SourcePort           = ConnectionStateTable[CSTableEntry].Port;
			TCPHeaderOUT->DestinationPort      = ConnectionStateTable[CSTableEntry].RemotePort;
			TCPHeaderOUT->SequenceNumber       = SwapEndian_32(ConnectionStateTable[CSTableEntry].Info.SequenceNumberOut);
			TCPHeaderOUT->AcknowledgmentNumber = SwapEndian_32(ConnectionStateTable[CSTableEntry].Info.SequenceNumberIn);
			TCPHeaderOUT->DataOffset           = (sizeof(TCP_Header_t) / sizeof(uint32_t));
			TCPHeaderOUT->WindowSize           = SwapEndian_16(TCP_WINDOW_SIZE);

			TCPHeaderOUT->Flags                = TCP_FLAG_ACK;
			TCPHeaderOUT->UrgentPointer        = 0;
			TCPHeaderOUT->Checksum             = 0;
			TCPHeaderOUT->Reserved             = 0;

			memcpy(TCPDataOUT, ConnectionStateTable[CSTableEntry].Info.Buffer.Data, PacketSize);
			
			ConnectionStateTable[CSTableEntry].Info.SequenceNumberOut += PacketSize;

			TCPHeaderOUT->Checksum             = TCP_Checksum16(TCPHeaderOUT, ServerIPAddress,
			                                                    ConnectionStateTable[CSTableEntry].RemoteAddress,
			                                                    (sizeof(TCP_Header_t) + PacketSize));

			PacketSize += sizeof(TCP_Header_t);

			/* Fill out the response IP header */
			IPHeaderOUT->TotalLength        = SwapEndian_16(sizeof(IP_Header_t) + PacketSize);
			IPHeaderOUT->TypeOfService      = 0;
			IPHeaderOUT->HeaderLength       = (sizeof(IP_Header_t) / sizeof(uint32_t));
			IPHeaderOUT->Version            = 4;
			IPHeaderOUT->Flags              = 0;
			IPHeaderOUT->FragmentOffset     = 0;
			IPHeaderOUT->Identification     = 0;
			IPHeaderOUT->HeaderChecksum     = 0;
			IPHeaderOUT->Protocol           = PROTOCOL_TCP;
			IPHeaderOUT->TTL                = DEFAULT_TTL;
			IPHeaderOUT->SourceAddress      = ServerIPAddress;
			IPHeaderOUT->DestinationAddress = ConnectionStateTable[CSTableEntry].RemoteAddress;
			
			IPHeaderOUT->HeaderChecksum     = Ethernet_Checksum16(IPHeaderOUT, sizeof(IP_Header_t));
		
			PacketSize += sizeof(IP_Header_t);
		
			/* Fill out the response Ethernet frame header */
			FrameOUTHeader->Source          = ServerMACAddress;
			FrameOUTHeader->Destination     = (MAC_Address_t){{0x02, 0x00, 0x02, 0x00, 0x02, 0x00}};
			FrameOUTHeader->EtherType       = SwapEndian_16(ETHERTYPE_IPV4);

			PacketSize += sizeof(Ethernet_Frame_Header_t);

			/* Set the response length in the buffer and indicate that a response is ready to be sent */
			FrameOUT.FrameLength            = PacketSize;
			FrameOUT.FrameInBuffer          = true;
			
			ConnectionStateTable[CSTableEntry].Info.Buffer.Ready = false;
			
			printf("RESPONSE SEND\r\n");
			
			break;
		}
	}
}

void TCP_Init(void)
{
	/* Initialize the port state table with all CLOSED entries */
	for (uint8_t PTableEntry = 0; PTableEntry < MAX_OPEN_TCP_PORTS; PTableEntry++)
	  PortStateTable[PTableEntry].State = TCP_Port_Closed;

	/* Initialize the connection table with all CLOSED entries */
	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
	  ConnectionStateTable[CSTableEntry].State = TCP_Connection_Closed;
}

bool TCP_SetPortState(uint16_t Port, uint8_t State, void (*Handler)(TCP_ConnectionBuffer_t*))
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t PTableEntry = 0; PTableEntry < MAX_TCP_CONNECTIONS; PTableEntry++)
	{
		/* Find existing entry for the port in the table, update it if found */
		if (PortStateTable[PTableEntry].Port == Port)
		{
			PortStateTable[PTableEntry].State = State;
			PortStateTable[PTableEntry].ApplicationHandler = Handler;
			return true;
		}
	}

	/* Check if trying to open the port -- if so we need to find an unused (closed) entry and replace it */
	if (State == TCP_Port_Open)
	{
		for (uint8_t PTableEntry = 0; PTableEntry < MAX_TCP_CONNECTIONS; PTableEntry++)
		{
			/* Find a closed port entry in the table, change it to the given port and state */
			if (PortStateTable[PTableEntry].State == TCP_Port_Closed)
			{
				PortStateTable[PTableEntry].Port  = Port;
				PortStateTable[PTableEntry].State = State;
				PortStateTable[PTableEntry].ApplicationHandler = Handler;
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

	for (uint8_t PTableEntry = 0; PTableEntry < MAX_TCP_CONNECTIONS; PTableEntry++)
	{
		/* Find existing entry for the port in the table, return the port status if found */
		if (PortStateTable[PTableEntry].Port == Port)
		  return PortStateTable[PTableEntry].State;
	}
	
	/* Port not in table, assume closed */
	return TCP_Port_Closed;
}
		
uint8_t TCP_GetConnectionState(uint16_t Port, IP_Address_t RemoteAddress, uint16_t RemotePort)
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
	{
		/* Find port entry in the table */
		if ((ConnectionStateTable[CSTableEntry].Port == Port) &&
		     IP_COMPARE(&ConnectionStateTable[CSTableEntry].RemoteAddress, &RemoteAddress) &&
			 ConnectionStateTable[CSTableEntry].RemotePort == RemotePort)
			 
		{
			return ConnectionStateTable[CSTableEntry].State;
		}
	}
	
	return TCP_Connection_Closed;
}

TCP_ConnectionInfo_t* TCP_GetConnectionInfo(uint16_t Port, IP_Address_t RemoteAddress, uint16_t RemotePort)
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
	{
		/* Find port entry in the table */
		if ((ConnectionStateTable[CSTableEntry].Port == Port) &&
		     IP_COMPARE(&ConnectionStateTable[CSTableEntry].RemoteAddress, &RemoteAddress) &&
			 ConnectionStateTable[CSTableEntry].RemotePort == RemotePort)
		{
			return &ConnectionStateTable[CSTableEntry].Info;
		}
	}
	
	return NULL;
}

bool TCP_SetConnectionState(uint16_t Port, IP_Address_t RemoteAddress, uint16_t RemotePort, uint8_t State)
{
	/* Note, Port number should be specified in BIG endian to simplfy network code */

	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
	{
		/* Find port entry in the table */
		if ((ConnectionStateTable[CSTableEntry].Port == Port) &&
		     IP_COMPARE(&ConnectionStateTable[CSTableEntry].RemoteAddress, &RemoteAddress) &&
			 ConnectionStateTable[CSTableEntry].RemotePort == RemotePort)
		{
			ConnectionStateTable[CSTableEntry].State = State;
			return true;
		}
	}
	
	for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
	{
		/* Find empty entry in the table */
		if (ConnectionStateTable[CSTableEntry].State == TCP_Connection_Closed)
		{
			ConnectionStateTable[CSTableEntry].Port          = Port;
			ConnectionStateTable[CSTableEntry].RemoteAddress = RemoteAddress;			
			ConnectionStateTable[CSTableEntry].RemotePort    = RemotePort;
			ConnectionStateTable[CSTableEntry].State         = State;
			return true;
		}
	}
	
	return false;
}

int16_t TCP_ProcessTCPPacket(void* IPHeaderInStart, void* TCPHeaderInStart, void* TCPHeaderOutStart)
{
	IP_Header_t*            IPHeaderIN   = (IP_Header_t*)IPHeaderInStart;
	TCP_Header_t*           TCPHeaderIN  = (TCP_Header_t*)TCPHeaderInStart;
	TCP_Header_t*           TCPHeaderOUT = (TCP_Header_t*)TCPHeaderOutStart;
	IP_Header_t*            IPHeaderIn   = (IP_Header_t*)IPHeaderInStart;

	TCP_ConnectionInfo_t*   ConnectionInfo;
	
	DecodeTCPHeader(TCPHeaderInStart);

	bool PacketResponse = false;
		
	/* Check if the destination port is open and allows incomming connections */
	if (TCP_GetPortState(TCPHeaderIN->DestinationPort) == TCP_Port_Open)
	{
		if (TCPHeaderIN->Flags == TCP_FLAG_SYN)
		  TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress, TCPHeaderIN->SourcePort, TCP_Connection_Closed);

		/* Process the incomming TCP packet based on the current connection state for the sender and port */
		switch (TCP_GetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress, TCPHeaderIN->SourcePort))
		{
			case TCP_Connection_Closed:
				if (TCPHeaderIN->Flags == TCP_FLAG_SYN)
				{
					/* SYN connection when closed starts a connection with a peer */

					TCPHeaderOUT->Flags = (TCP_FLAG_SYN | TCP_FLAG_ACK);				
					PacketResponse      = true;
								
					TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress, TCPHeaderIN->SourcePort,
										   TCP_Connection_SYNReceived);
										   
					ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress, TCPHeaderIN->SourcePort);

					ConnectionInfo->SequenceNumberIn  = (SwapEndian_32(TCPHeaderIN->SequenceNumber) + 1);
					ConnectionInfo->SequenceNumberOut = 0;
					ConnectionInfo->Buffer.InUse      = false;

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
					/* ACK during the connection process completes the connection to a peer */

					TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
										   TCPHeaderIN->SourcePort, TCP_Connection_Established);

					ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
					                                       TCPHeaderIN->SourcePort);
														   
					ConnectionInfo->SequenceNumberOut++;

					printf("SYNRECEIVED->ESTABLISHED\r\n");
				}
				else
				{
					printf("SYNRECEIVED->SELF\r\n");				
				}
				
				break;
			case TCP_Connection_Established:
				if (TCPHeaderIN->Flags == (TCP_FLAG_FIN | TCP_FLAG_ACK))
				{
					/* FYN ACK when connected to a peer starts the finalization process */
				
					TCPHeaderOUT->Flags = (TCP_FLAG_FIN | TCP_FLAG_ACK);				
					PacketResponse      = true;
					
					TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
										   TCPHeaderIN->SourcePort, TCP_Connection_CloseWait);

					ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
					                                       TCPHeaderIN->SourcePort);

					ConnectionInfo->SequenceNumberIn++;
					ConnectionInfo->SequenceNumberOut++;

					printf("ESTABLISHED->CLOSEWAIT\r\n");
				}
				else if ((TCPHeaderIN->Flags == TCP_FLAG_ACK) || (TCPHeaderIN->Flags == (TCP_FLAG_ACK | TCP_FLAG_PSH)))
				{
					printf("ESTABLISHED->SELF\r\n");

					ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
					                                       TCPHeaderIN->SourcePort);

					/* Check if the buffer is currently in use either by a buffered data to send, or receive */		
					if ((ConnectionInfo->Buffer.InUse == false) && (ConnectionInfo->Buffer.Ready == false))
					{						
						ConnectionInfo->Buffer.Direction = TCP_PACKETDIR_IN;
						ConnectionInfo->Buffer.InUse     = true;
						ConnectionInfo->Buffer.Length    = 0;
					}
					
					/* Check if the buffer has been claimed by us to read in data from the peer */
					if ((ConnectionInfo->Buffer.Direction == TCP_PACKETDIR_IN) &&
					    (ConnectionInfo->Buffer.Length != TCP_WINDOW_SIZE))
					{
						uint16_t IPOffset   = (IPHeaderIN->HeaderLength * sizeof(uint32_t));
						uint16_t TCPOffset  = (TCPHeaderIN->DataOffset * sizeof(uint32_t));
						uint16_t DataLength = (SwapEndian_16(IPHeaderIn->TotalLength) - IPOffset - TCPOffset);

						/* Copy the packet data into the buffer */
						memcpy(&ConnectionInfo->Buffer.Data[ConnectionInfo->Buffer.Length],
							   &((uint8_t*)TCPHeaderInStart)[TCPOffset],
							   DataLength);

						ConnectionInfo->SequenceNumberIn += DataLength;
						ConnectionInfo->Buffer.Length    += DataLength;
						
						/* Check if the buffer is full or if the PSH flag is set, if so indicate buffer ready */
						if ((!(TCP_WINDOW_SIZE - ConnectionInfo->Buffer.Length)) || (TCPHeaderIN->Flags & TCP_FLAG_PSH))
						{
							ConnectionInfo->Buffer.InUse = false;
							ConnectionInfo->Buffer.Ready = true;

							TCPHeaderOUT->Flags = TCP_FLAG_ACK;
							PacketResponse      = true;
						}
					}
					else
					{
						printf("Processing deferred, buffer full.\r\n");
						return NO_PROCESS;
					}
				}
				
				break;
			case TCP_Connection_CloseWait:
				if (TCPHeaderIN->Flags == TCP_FLAG_ACK)
				{
					TCP_SetConnectionState(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
										   TCPHeaderIN->SourcePort, TCP_Connection_Closed);

					printf("CLOSEWAIT->CLOSED\r\n");
				}
				else
				{
					printf("CLOSEWAIT->SELF\r\n");			
				}
				
				break;			
		}
	}
	else
	{
		/* Port is not open, indicate via a RST/ACK response to the sender */
		TCPHeaderOUT->Flags = (TCP_FLAG_RST | TCP_FLAG_ACK);				
		PacketResponse      = true;
	}
	
	/* Check if we need to respond to the sent packet */
	if (PacketResponse)
	{
		ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, IPHeaderIN->SourceAddress,
		                                       TCPHeaderIN->SourcePort);

		TCPHeaderOUT->SourcePort           = TCPHeaderIN->DestinationPort;
		TCPHeaderOUT->DestinationPort      = TCPHeaderIN->SourcePort;
		TCPHeaderOUT->SequenceNumber       = SwapEndian_32(ConnectionInfo->SequenceNumberOut);
		TCPHeaderOUT->AcknowledgmentNumber = SwapEndian_32(ConnectionInfo->SequenceNumberIn);
		TCPHeaderOUT->DataOffset           = (sizeof(TCP_Header_t) / sizeof(uint32_t));
		
		if (!(ConnectionInfo->Buffer.InUse))
		  TCPHeaderOUT->WindowSize         = SwapEndian_16(TCP_WINDOW_SIZE);
		else
		  TCPHeaderOUT->WindowSize         = SwapEndian_16(TCP_WINDOW_SIZE - ConnectionInfo->Buffer.Length);

		TCPHeaderOUT->UrgentPointer        = 0;
		TCPHeaderOUT->Checksum             = 0;
		TCPHeaderOUT->Reserved             = 0;
		
		TCPHeaderOUT->Checksum             = TCP_Checksum16(TCPHeaderOUT, IPHeaderIn->DestinationAddress,
		                                                    IPHeaderIn->SourceAddress, sizeof(TCP_Header_t));					

		return sizeof(TCP_Header_t);	
	}

	return NO_RESPONSE;
}

static uint16_t TCP_Checksum16(void* TCPHeaderOutStart, IP_Address_t SourceAddress,
                               IP_Address_t DestinationAddress, uint16_t TCPOutSize)
{
	uint32_t Checksum = 0;
	
	/* TCP/IP checksums are the addition of the one's compliment of each word including the IP psudo-header,
	   complimented */
	
	Checksum += ((uint16_t*)&SourceAddress)[0];
	Checksum += ((uint16_t*)&SourceAddress)[1];
	Checksum += ((uint16_t*)&DestinationAddress)[0];
	Checksum += ((uint16_t*)&DestinationAddress)[1];
	Checksum += SwapEndian_16(PROTOCOL_TCP);
	Checksum += SwapEndian_16(TCPOutSize);

	for (uint8_t CurrWord = 0; CurrWord < (TCPOutSize >> 1); CurrWord++)
	  Checksum += ((uint16_t*)TCPHeaderOutStart)[CurrWord];
	
	if (TCPOutSize & 0x01)
	  Checksum += (((uint16_t*)TCPHeaderOutStart)[TCPOutSize >> 1] & 0x00FF);
	  
	while (Checksum & 0xFFFF0000)
	  Checksum = ((Checksum & 0xFFFF) + (Checksum >> 16));
	
	return ~Checksum;
}
