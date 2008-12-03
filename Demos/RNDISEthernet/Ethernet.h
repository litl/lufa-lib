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

#ifndef _ETHERNET_H_
#define _ETHERNET_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>

		#include "EthernetProtocols.h"
		#include "ProtocolDecoders.h"
		#include "ICMP.h"
		#include "TCP.h"
		#include "UDP.h"
		#include "DHCP.h"
		#include "ARP.h"
		#include "IP.h"
		
	/* Macros: */
		#define SERVER_MAC_ADDRESS               {0x00, 0x01, 0x00, 0x01, 0x00, 0x01}		
		#define BROADCAST_MAC_ADDRESS            {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
	
		#define MAC_COMPARE(mac1, mac2)          (memcmp(mac1, mac2, sizeof(MAC_Address_t)) == 0)

		#define ETHERNET_FRAME_SIZE_MAX          1500
		#define ETHERNET_FRAME_SIZE_MIN          64
		
		#define ETHERNET_VER2_MINSIZE            0x0600
		
		#define NO_RESPONSE                      0		
		#define NO_PROCESS                       -1

	/* Type Defines: */
		typedef struct
		{
			uint8_t       FrameData[ETHERNET_FRAME_SIZE_MAX];
			uint16_t      FrameLength;
			bool          FrameInBuffer;			
		} Ethernet_Frame_Info_t;

		typedef struct
		{
			MAC_Address_t Destination;
			MAC_Address_t Source;
			
			union
			{
				uint16_t  EtherType;
				uint16_t  Length;
			};
		} Ethernet_Frame_Header_t;
		
	/* External Variables: */
		extern Ethernet_Frame_Info_t FrameIN;
		extern Ethernet_Frame_Info_t FrameOUT;

		extern MAC_Address_t ServerMACAddress;
		extern IP_Address_t  ServerIPAddress;
		extern MAC_Address_t BroadcastMACAddress;
		extern IP_Address_t  BroadcastIPAddress;
		
	/* Function Prototypes: */
		void     Ethernet_ProcessPacket(void);
		uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes);
		
#endif
