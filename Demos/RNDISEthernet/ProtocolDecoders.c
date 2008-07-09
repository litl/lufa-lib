/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "ProtocolDecoders.h"

void DecodeEthernetFrameHeader(void* InDataStart)
{
	Ethernet_Frame_Header_t* FrameHeader = (Ethernet_Frame_Header_t*)InDataStart;
	
	printf("\r\n");
	
	printf("  ETHERNET\r\n");
	printf("  + Frame Size: %d\r\n", FrameIN.FrameLength);

	if (!(MAC_COMPARE(&FrameHeader->Destination, &ServerMACAddress)) &&
	    !(MAC_COMPARE(&FrameHeader->Destination, &BroadcastMACAddress)))
	{
		printf("  + NOT ADDRESSED TO DEVICE\r\n");
		return;
	}

	printf("  + MAC Source : %02X:%02X:%02X:%02X:%02X:%02X\r\n", FrameHeader->Source.Octets[0],
	                                                             FrameHeader->Source.Octets[1],
	                                                             FrameHeader->Source.Octets[2],
	                                                             FrameHeader->Source.Octets[3],
	                                                             FrameHeader->Source.Octets[4],
	                                                             FrameHeader->Source.Octets[5]);

	printf("  + MAC Dest: %02X:%02X:%02X:%02X:%02X:%02X\r\n",    FrameHeader->Destination.Octets[0],
	                                                             FrameHeader->Destination.Octets[1],
	                                                             FrameHeader->Destination.Octets[2],
	                                                             FrameHeader->Destination.Octets[3],
	                                                             FrameHeader->Destination.Octets[4],
	                                                             FrameHeader->Destination.Octets[5]);

	if (SwapEndian_16(FrameIN.FrameLength) > ETHERNET_VER2_MINSIZE)
	  printf("  + Protocol: 0x%04x\r\n", SwapEndian_16(FrameHeader->EtherType));
	else
	  printf("  + Protocol: UNKNOWN E1\r\n");
}

void DecodeARPHeader(void* InDataStart)
{
	Ethernet_ARP_Header_t* ARPHeader = (Ethernet_ARP_Header_t*)InDataStart;	

	printf("   \\\r\n    ARP\r\n");

	bool IsIPtoMAC = MAC_COMPARE(&ARPHeader->THA, &NullMACAddress);

	if ((IsIPtoMAC && !IP_COMPARE(&ARPHeader->TPA, &ServerIPAddress)) || 
	   (!(IsIPtoMAC) && !MAC_COMPARE(&ARPHeader->THA, &ServerMACAddress)))
	{
		printf("    + NOT ADDRESSED TO DEVICE\r\n");
		return;		
	}

	printf("    + Protocol: %x\r\n", SwapEndian_16(ARPHeader->ProtocolType));
	printf("    + Operation: %d\r\n", SwapEndian_16(ARPHeader->Operation));
	
	if (SwapEndian_16(ARPHeader->ProtocolType) == ETHERTYPE_IPV4)
	{
		printf("    + SHA MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", ARPHeader->SHA.Octets[0],
		                                                           ARPHeader->SHA.Octets[1],
		                                                           ARPHeader->SHA.Octets[2],
		                                                           ARPHeader->SHA.Octets[3],
		                                                           ARPHeader->SHA.Octets[4],
		                                                           ARPHeader->SHA.Octets[5]);

		printf("    + SPA IP: %d.%d.%d.%d\r\n",                    ARPHeader->SPA.Octets[0],
		                                                           ARPHeader->SPA.Octets[1],
		                                                           ARPHeader->SPA.Octets[2],
		                                                           ARPHeader->SPA.Octets[3]);

		printf("    + THA MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", ARPHeader->THA.Octets[0],
		                                                           ARPHeader->THA.Octets[1],
		                                                           ARPHeader->THA.Octets[2],
		                                                           ARPHeader->THA.Octets[3],
		                                                           ARPHeader->THA.Octets[4],
		                                                           ARPHeader->THA.Octets[5]);

		printf("    + TPA IP: %d.%d.%d.%d\r\n",                    ARPHeader->TPA.Octets[0],
		                                                           ARPHeader->TPA.Octets[1],
		                                                           ARPHeader->TPA.Octets[2],
		                                                           ARPHeader->TPA.Octets[3]);
	}
}

void DecodeIPHeader(void* InDataStart)
{
	Ethernet_IP_Header_t* IPHeader  = (Ethernet_IP_Header_t*)InDataStart;

	uint16_t              HeaderLengthBytes = (IPHeader->HeaderLength * sizeof(uint32_t));

	printf("   \\\r\n    IP\r\n");

	if (!(IP_COMPARE(&IPHeader->SourceAddress, &ServerIPAddress)))
	{
		printf("    + NOT ADDRESSED TO DEVICE\r\n");
		return;
	}

	printf("    + Packet Version: %d\r\n", IPHeader->Version);
	printf("    + Header Length: %d Bytes\r\n", HeaderLengthBytes);
	printf("    + Total Length: %d\r\n", SwapEndian_16(IPHeader->TotalLength));
	
	printf("    + Protocol: %d\r\n", IPHeader->Protocol);
	printf("    + TTL: %d\r\n", IPHeader->TTL);
	
	printf("    + IP Src: %d.%d.%d.%d\r\n", IPHeader->SourceAddress.Octets[0],
	                                        IPHeader->SourceAddress.Octets[1],
	                                        IPHeader->SourceAddress.Octets[2],
	                                        IPHeader->SourceAddress.Octets[3]);	

	printf("    + IP Dst: %d.%d.%d.%d\r\n", IPHeader->DestinationAddress.Octets[0],
	                                        IPHeader->DestinationAddress.Octets[1],
	                                        IPHeader->DestinationAddress.Octets[2],
	                                        IPHeader->DestinationAddress.Octets[3]);	
}

void DecodeICMPHeader(void* InDataStart)
{
	Ethernet_ICMP_Header_t* ICMPHeader  = (Ethernet_ICMP_Header_t*)InDataStart;

	printf("    \\\r\n     ICMP\r\n");

	printf("     + Type: %d\r\n", ICMPHeader->Type);
	printf("     + Code: %d\r\n", ICMPHeader->Code);
}

void DecodeTCPHeader(void* InDataStart)
{
	printf("    \\\r\n     TCP\r\n");
}
