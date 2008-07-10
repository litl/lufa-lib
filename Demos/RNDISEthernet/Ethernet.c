/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

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
				RetSize = ARP_ProcessARPPacket(&FrameIN.FrameData[sizeof(Ethernet_Frame_Header_t)],
				                               &FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t)]);
				break;		
			case ETHERTYPE_IPV4:
				RetSize = IP_ProcessIPPacket(&FrameIN.FrameData[sizeof(Ethernet_Frame_Header_t)],
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

uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes)
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
