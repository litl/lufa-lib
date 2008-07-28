/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
		/* If the ARP packet is requesting the MAC or IP of the virtual webserver, return the response */
		if (IP_COMPARE(&ARPHeaderIN->TPA, &ServerIPAddress) || 
		    MAC_COMPARE(&ARPHeaderIN->THA, &ServerMACAddress))
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

			/* Return the size of the response so far */
			return sizeof(ARP_Header_t);
		}
	}
	
	return NO_RESPONSE;
}
