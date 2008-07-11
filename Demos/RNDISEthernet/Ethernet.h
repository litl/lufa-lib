/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
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
		extern MAC_Address_t NullMACAddress;
		extern MAC_Address_t BroadcastMACAddress;
		
	/* Function Prototypes: */
		void     Ethernet_ProcessPacket(void);
		uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes);
		
#endif
