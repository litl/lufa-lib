/*
             LUFA Library
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

#include "DHCP.h"

int16_t DHCP_ProcessDHCPPacket(void* IPHeaderInStart, void* DHCPHeaderInStart, void* DHCPHeaderOutStart)
{
	IP_Header_t*   IPHeaderIN    = (IP_Header_t*)IPHeaderInStart;
	DHCP_Header_t* DHCPHeaderIN  = (DHCP_Header_t*)DHCPHeaderInStart;
	DHCP_Header_t* DHCPHeaderOUT = (DHCP_Header_t*)DHCPHeaderOutStart;
	
	uint8_t* DHCPOptionsINStart  = (uint8_t*)(DHCPHeaderInStart  + sizeof(DHCP_Header_t));
	uint8_t* DHCPOptionsOUTStart = (uint8_t*)(DHCPHeaderOutStart + sizeof(DHCP_Header_t));

	DecodeDHCPHeader(DHCPHeaderInStart);

	/* Zero out the response DHCP packet, as much of it legacy and left at 0 */
	memset(DHCPHeaderOUT, 0, sizeof(DHCP_Header_t));

	/* Fill out the response DHCP packet */
	DHCPHeaderOUT->HardwareType          = DHCPHeaderIN->HardwareType;
	DHCPHeaderOUT->Operation             = DHCP_OP_BOOTREPLY;
	DHCPHeaderOUT->HardwareAddressLength = DHCPHeaderIN->HardwareAddressLength;
	DHCPHeaderOUT->Hops                  = 0;
	DHCPHeaderOUT->TransactionID         = DHCPHeaderIN->TransactionID;
	DHCPHeaderOUT->ElapsedSeconds        = 0;
	DHCPHeaderOUT->Flags                 = DHCPHeaderIN->Flags;
	memcpy(&DHCPHeaderOUT->YourIP, &ClientIPAddress, sizeof(IP_Address_t));
	memcpy(&DHCPHeaderOUT->ClientHardwareAddress, &DHCPHeaderIN->ClientHardwareAddress, sizeof(MAC_Address_t));
	DHCPHeaderOUT->Cookie                = SwapEndian_32(DHCP_MAGIC_COOKIE);
	
	/* Alter the incomming IP packet header so that the corrected IP source and destinations are used - this means that
	   when the response IP header is generated, it will use the corrected addresses and not the null/broatcast addresses */
	memcpy(&IPHeaderIN->SourceAddress, &ClientIPAddress, sizeof(IP_Address_t));
	memcpy(&IPHeaderIN->DestinationAddress, &ServerIPAddress, sizeof(IP_Address_t));

	/* Process the incomming DHCP packet options */
	while (DHCPOptionsINStart[0] != DHCP_OPTION_END)
	{	
		/* Find the Message Type DHCP option, to determine the type of DHCP packet */
		if (DHCPOptionsINStart[0] == DHCP_OPTION_MESSAGETYPE)
		{
			switch (DHCPOptionsINStart[2])
			{
				case DHCP_MESSAGETYPE_DISCOVER:
					/* Fill out the response DHCP packet options for a DHCP OFFER response */

					*(DHCPOptionsOUTStart++) = DHCP_OPTION_MESSAGETYPE;
					*(DHCPOptionsOUTStart++) = 1;
					*(DHCPOptionsOUTStart++) = DHCP_MESSAGETYPE_OFFER;

					*(DHCPOptionsOUTStart++) = DHCP_OPTION_SUBNETMASK;
					*(DHCPOptionsOUTStart++) = 4;
					*(DHCPOptionsOUTStart++) = 0xFF;
					*(DHCPOptionsOUTStart++) = 0xFF;
					*(DHCPOptionsOUTStart++) = 0xFF;
					*(DHCPOptionsOUTStart++) = 0x00;

					*(DHCPOptionsOUTStart++) = DHCP_OPTION_END;
					
					return (sizeof(DHCP_Header_t) + 10);
				case DHCP_MESSAGETYPE_REQUEST:
					/* Fill out the response DHCP packet options for a DHCP ACK response */

					*(DHCPOptionsOUTStart++) = DHCP_OPTION_MESSAGETYPE;
					*(DHCPOptionsOUTStart++) = 1;
					*(DHCPOptionsOUTStart++) = DHCP_MESSAGETYPE_ACK;
					
					*(DHCPOptionsOUTStart++) = DHCP_OPTION_END;
					
					return (sizeof(DHCP_Header_t) + 4);
			}
		}
		
		/* Go to the next DHCP option - skip one byte if option is a padding byte, else skip the complete option's size */
		DHCPOptionsINStart += ((DHCPOptionsINStart[0] == DHCP_OPTION_PAD) ? 1 : (DHCPOptionsINStart[1] + 2));
	}
	
	return NO_RESPONSE;
}
