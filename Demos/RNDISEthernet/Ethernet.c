/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/* Simple Ethernet protocol processing stack. This module provides functions for decomposing an Ethernet
   frame into its component protocols, as well as handlers for the more simple protocols, namely:
   
   Ethernet
    \
     + ARP (IP to MAC, MAC to IP)
     + IP
       \
        + ICMP (Echo Requests and Responses)
	    + TCP (In-order reliable packet traffic)
	      \
		   + HTTP (Webserver)

	The TCP protocol is too complex to exist in this module, thus it has been moved to a module of
	its own. The webserver, too, exists as a seperate module and runs upon the TCP protocol.
*/

#define   INCLUDE_FROM_ETHERNET_C
#include "Ethernet.h"

/* Global Variables: */
Ethernet_Frame_Info_t FrameIN;
Ethernet_Frame_Info_t FrameOUT;

MAC_Address_t ServerMACAddress    = {SERVER_MAC_ADDRESS};
IP_Address_t  ServerIPAddress     = {SERVER_IP_ADDRESS};
MAC_Address_t BroadcastMACAddress = {BROADCAST_MAC_ADDRESS};


void Ethernet_ProcessPacket(void)
{
	DecodeEthernetFrameHeader(FrameIN.FrameData);

	/* Cast the incomming Ethernet frame to the Ethernet header type */
	Ethernet_Frame_Header_t* FrameINHeader  = (Ethernet_Frame_Header_t*)&FrameIN.FrameData;
	Ethernet_Frame_Header_t* FrameOUTHeader = (Ethernet_Frame_Header_t*)&FrameOUT.FrameData;
	
	uint16_t                 RetSize        = NO_RESPONSE;
	
	FrameIN.FrameLength -= sizeof(Ethernet_Frame_Header_t);

	/* Ensure frame is addressed to either all (broadcast) or the virtual webserver */
	if (!(MAC_COMPARE(&FrameINHeader->Destination, &ServerMACAddress)) &&
	    !(MAC_COMPARE(&FrameINHeader->Destination, &BroadcastMACAddress)))
	{
		return;
	}

	/* If the packet is of the Ethernet II type, process it */
	if (SwapEndian_16(FrameIN.FrameLength) > ETHERNET_VER2_MINSIZE)
	{	
		/* Process the packet depending on its protocol */
		switch (SwapEndian_16(FrameINHeader->EtherType))
		{
			case ETHERTYPE_ARP:
				RetSize = Ethernet_ProcessARPPacket(&FrameIN.FrameData[sizeof(Ethernet_Frame_Header_t)],
				                                    &FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t)]);
				break;		
			case ETHERTYPE_IPV4:
				RetSize = Ethernet_ProcessIPPacket(&FrameIN.FrameData[sizeof(Ethernet_Frame_Header_t)],
				                                   &FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t)]);
				break;
		}
		
		/* Protcol processing routine has filled a response, complete the ethernet frame header */
		if (RetSize != NO_RESPONSE)
		{
			/* Fill out the response Ethernet frame header */
			memcpy(&FrameOUTHeader->Source, &ServerMACAddress, sizeof(MAC_Address_t));
			FrameOUTHeader->Destination     = FrameINHeader->Source;
			FrameOUTHeader->EtherType       = FrameINHeader->EtherType;			
			
			/* Set the response length in the buffer and indicate that a response is ready to be sent */
			FrameOUT.FrameLength            = (sizeof(Ethernet_Frame_Header_t) + RetSize);
			FrameOUT.FrameInBuffer          = true;
		}
	}
}

static uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes)
{
	uint16_t* Words    = (uint16_t*)Data;
	union
	{
		uint32_t  DWord;
		uint16_t  Words[2];
	} Checksum = {0};

	
	/* TCP/IP checksums are the addition of the one's compliment of each word, complimented */
	
	for (uint8_t CurrWord = 0; CurrWord < (Bytes >> 1); CurrWord++)
	  Checksum.DWord += Words[CurrWord];
	  
	return ~(Checksum.Words[0] + Checksum.Words[1]);
}

static uint16_t Ethernet_ProcessARPPacket(void* InDataStart, void* OutDataStart)
{
	DecodeARPHeader(InDataStart);

	Ethernet_ARP_Header_t* ARPHeaderIN  = (Ethernet_ARP_Header_t*)InDataStart;
	Ethernet_ARP_Header_t* ARPHeaderOUT = (Ethernet_ARP_Header_t*)OutDataStart;

	FrameIN.FrameLength -= sizeof(Ethernet_ARP_Header_t);

	/* Ensure that the ARP request is a IPv4 request packet */
	if ((SwapEndian_16(ARPHeaderIN->ProtocolType) == ETHERTYPE_IPV4) &&
	    (SwapEndian_16(ARPHeaderIN->Operation) == ARP_OPERATION_REQUEST))
	{
		/* Fill out the ARP response header */
		ARPHeaderOUT->HardwareType = ARPHeaderIN->HardwareType;
		ARPHeaderOUT->ProtocolType = ARPHeaderIN->ProtocolType;
		ARPHeaderOUT->HLEN         = ARPHeaderIN->HLEN;
		ARPHeaderOUT->PLEN         = ARPHeaderIN->PLEN;
		ARPHeaderOUT->Operation    = SwapEndian_16(ARP_OPERATION_REPLY);

		/* Copy over the sender MAC/IP to the target fields for the response */
		memcpy(&ARPHeaderOUT->THA, &ARPHeaderIN->SHA, sizeof(MAC_Address_t));
		memcpy(&ARPHeaderOUT->TPA, &ARPHeaderIN->SPA, sizeof(IP_Address_t));

		/* Copy over the new sender MAC/IP - MAC and IP addresses of the virtual webserver */
		memcpy(&ARPHeaderOUT->SHA, &ServerMACAddress, sizeof(MAC_Address_t));
		memcpy(&ARPHeaderOUT->SPA, &ServerIPAddress, sizeof(IP_Address_t));

		/* If the ARP packet is requesting the MAC or IP of the virtual webserver, return the response */
		if (IP_COMPARE(&ARPHeaderIN->TPA, &ServerIPAddress) || 
		    MAC_COMPARE(&ARPHeaderIN->THA, &ServerMACAddress))
		{
			/* Return the size of the response so far */
			return sizeof(Ethernet_ARP_Header_t);
		}
	}
	
	return NO_RESPONSE;
}

static uint16_t Ethernet_ProcessIPPacket(void* InDataStart, void* OutDataStart)
{
	DecodeIPHeader(InDataStart);

	Ethernet_IP_Header_t* IPHeaderIN  = (Ethernet_IP_Header_t*)InDataStart;
	Ethernet_IP_Header_t* IPHeaderOUT = (Ethernet_IP_Header_t*)OutDataStart;

	/* Header length is specified in number of longs in the packet header, convert to bytes */
	uint16_t              HeaderLengthBytes = (IPHeaderIN->HeaderLength * sizeof(uint32_t));

	uint16_t              RetSize     = NO_RESPONSE;

	FrameIN.FrameLength -= HeaderLengthBytes;

	/* Check to ensure the IP packet is addressed to the virtual webserver's IP */
	if (!(IP_COMPARE(&IPHeaderIN->DestinationAddress, &ServerIPAddress)))
	  return NO_RESPONSE;
	
	/* Pass off the IP payload to the appropriate protocol processing routine */
	switch (IPHeaderIN->Protocol)
	{
		case PROTOCOL_ICMP:
			RetSize = Ethernet_ProcessICMPPacket(&((uint8_t*)InDataStart)[HeaderLengthBytes],
			                                     &((uint8_t*)OutDataStart)[sizeof(Ethernet_IP_Header_t)]);
			break;
		case PROTOCOL_TCP:
			RetSize = Ethernet_ProcessTCPPacket(&((uint8_t*)InDataStart)[HeaderLengthBytes],
			                                    &((uint8_t*)OutDataStart)[sizeof(Ethernet_IP_Header_t)]);		
			break;
	}
	
	/* Check to see if the protocol processing routine has filled out a response */
	if (RetSize != NO_RESPONSE)
	{
		/* Fill out the response IP packet header */
		IPHeaderOUT->TotalLength        = SwapEndian_16(sizeof(Ethernet_IP_Header_t) + RetSize);
		IPHeaderOUT->TypeOfService      = 0;
		IPHeaderOUT->HeaderLength       = (sizeof(Ethernet_IP_Header_t) / sizeof(uint32_t));
		IPHeaderOUT->Version            = 4;
		IPHeaderOUT->Flags              = 0;
		IPHeaderOUT->FragmentOffset     = 0;
		IPHeaderOUT->Identification     = 0;
		IPHeaderOUT->HeaderChecksum     = 0;
		IPHeaderOUT->Protocol           = IPHeaderIN->Protocol;
		IPHeaderOUT->TTL                = DEFAULT_TTL;
		IPHeaderOUT->SourceAddress      = IPHeaderIN->DestinationAddress;
		IPHeaderOUT->DestinationAddress = IPHeaderIN->SourceAddress;
		
		IPHeaderOUT->HeaderChecksum     = Ethernet_Checksum16(IPHeaderOUT, sizeof(Ethernet_IP_Header_t));
						
		/* Return the size of the response so far */
		return (sizeof(Ethernet_IP_Header_t) + RetSize);
	}
	
	return NO_RESPONSE;
}

static uint16_t Ethernet_ProcessICMPPacket(void* InDataStart, void* OutDataStart)
{
	DecodeICMPHeader(InDataStart);

	Ethernet_ICMP_Header_t* ICMPHeaderIN  = (Ethernet_ICMP_Header_t*)InDataStart;
	Ethernet_ICMP_Header_t* ICMPHeaderOUT = (Ethernet_ICMP_Header_t*)OutDataStart;

	FrameIN.FrameLength -= sizeof(Ethernet_ICMP_Header_t);

	/* Determine if the ICMP packet is an echo request (ping) */
	if (ICMPHeaderIN->Type == ICMP_TYPE_ECHOREQUEST)
	{
		/* Fill out the ICMP response packet */
		ICMPHeaderOUT->Type     = ICMP_TYPE_ECHOREPLY;
		ICMPHeaderOUT->Code     = ICMP_ECHOREPLY_ECHOREPLY;
		ICMPHeaderOUT->Checksum = 0;
		ICMPHeaderOUT->Id       = ICMPHeaderIN->Id;
		ICMPHeaderOUT->Sequence = ICMPHeaderIN->Sequence;

		ICMPHeaderOUT->Checksum = Ethernet_Checksum16(ICMPHeaderOUT, sizeof(Ethernet_ICMP_Header_t));
		
		/* Copy the remaining payload to the response - echo requests should echo back any sent data */
		memcpy(&((uint8_t*)OutDataStart)[sizeof(Ethernet_ICMP_Header_t)],
		       &((uint8_t*)InDataStart)[sizeof(Ethernet_ICMP_Header_t)],
			   FrameIN.FrameLength);

		FrameIN.FrameLength = 0;

		/* Return the size of the response so far */
		return (sizeof(Ethernet_ICMP_Header_t) + FrameIN.FrameLength);
	}
	
	return NO_RESPONSE;
}

uint16_t Ethernet_ProcessTCPPacket(void* InDataStart, void* OutDataStart)
{
	DecodeTCPHeader(InDataStart);

	return TCP_ProcessTCPPacket(InDataStart, OutDataStart);
}
