/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "ARP.h"

int16_t ARP_ProcessARPPacket(void* InDataStart, void* OutDataStart)
{
	DecodeARPHeader(InDataStart);

	ARP_Header_t* ARPHeaderIN  = (ARP_Header_t*)InDataStart;
	ARP_Header_t* ARPHeaderOUT = (ARP_Header_t*)OutDataStart;

	FrameIN.FrameLength -= sizeof(ARP_Header_t);

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
		ARPHeaderOUT->THA = ARPHeaderIN->SHA;
		ARPHeaderOUT->TPA = ARPHeaderIN->SPA;

		/* Copy over the new sender MAC/IP - MAC and IP addresses of the virtual webserver */
		ARPHeaderOUT->SHA = ServerMACAddress;
		ARPHeaderOUT->SPA = ServerIPAddress;

		/* If the ARP packet is requesting the MAC or IP of the virtual webserver, return the response */
		if (IP_COMPARE(&ARPHeaderIN->TPA, &ServerIPAddress) || 
		    MAC_COMPARE(&ARPHeaderIN->THA, &ServerMACAddress))
		{
			/* Return the size of the response so far */
			return sizeof(ARP_Header_t);
		}
	}
	
	return NO_RESPONSE;
}
