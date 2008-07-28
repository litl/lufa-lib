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

#include "ICMP.h"

int16_t ICMP_ProcessICMPPacket(void* InDataStart, void* OutDataStart)
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
