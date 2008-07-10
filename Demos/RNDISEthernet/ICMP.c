/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "ICMP.h"

uint16_t ICMP_ProcessICMPPacket(void* InDataStart, void* OutDataStart)
{
	ICMP_Header_t* ICMPHeaderIN  = (ICMP_Header_t*)InDataStart;
	ICMP_Header_t* ICMPHeaderOUT = (ICMP_Header_t*)OutDataStart;

	DecodeICMPHeader(InDataStart);

	FrameIN.FrameLength -= sizeof(ICMP_Header_t);

	/* Determine if the ICMP packet is an echo request (ping) */
	if (ICMPHeaderIN->Type == ICMP_TYPE_ECHOREQUEST)
	{
		/* Fill out the ICMP response packet */
		ICMPHeaderOUT->Type     = ICMP_TYPE_ECHOREPLY;
		ICMPHeaderOUT->Code     = ICMP_ECHOREPLY_ECHOREPLY;
		ICMPHeaderOUT->Checksum = 0;
		ICMPHeaderOUT->Id       = ICMPHeaderIN->Id;
		ICMPHeaderOUT->Sequence = ICMPHeaderIN->Sequence;

		ICMPHeaderOUT->Checksum = Ethernet_Checksum16(ICMPHeaderOUT, sizeof(ICMP_Header_t));
		
		/* Copy the remaining payload to the response - echo requests should echo back any sent data */
		memcpy(&((uint8_t*)OutDataStart)[sizeof(ICMP_Header_t)],
		       &((uint8_t*)InDataStart)[sizeof(ICMP_Header_t)],
			   FrameIN.FrameLength);

		FrameIN.FrameLength = 0;

		/* Return the size of the response so far */
		return (sizeof(ICMP_Header_t) + FrameIN.FrameLength);
	}
	
	return NO_RESPONSE;
}
