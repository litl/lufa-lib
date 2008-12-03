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

#ifndef _UDP_H_
#define _UDP_H_

	/* Includes: */
		#include <avr/io.h>
	
		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"
	
	/* Macros: */
		#define UDP_PORT_DHCP_REQUEST 67
		#define UDP_PORT_DHCP_REPLY   68
			
	/* Type Defines: */
		typedef struct
		{
			uint16_t SourcePort;
			uint16_t DestinationPort;
			uint16_t Length;
			uint16_t Checksum;
		} UDP_Header_t;
		
	/* Function Prototypes: */
		int16_t UDP_ProcessUDPPacket(void* IPHeaderInStart, void* UDPHeaderInStart, void* UDPHeaderOutStart);
		
		#if defined(INCLUDE_FROM_UDP_C)
		static uint16_t UDP_Checksum16(void* UDPHeaderOutStart, IP_Address_t SourceAddress,
                                       IP_Address_t DestinationAddress, uint16_t UDPOutSize);
		#endif

#endif
