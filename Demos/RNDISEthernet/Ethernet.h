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
		
		#include "RNDIS.h"
		#include "EthernetProtocols.h"
		#include "ProtocolDecoders.h"
		
	/* Macros: */
		#define SERVER_MAC_ADDRESS               {0x40, 0x02, 0x04, 0x06, 0x08, 0x0A}
		#define SERVER_IP_ADDRESS                {192, 168, 0, 30}
		
		#define NULL_MAC_ADDRESS                 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
		#define BROADCAST_MAC_ADDRESS            {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
	
		#define ETHERNET_FRAME_SIZE_MAX          1522
		#define ETHERNET_FRAME_SIZE_MIN          64
		
		#define ETHERNET_VER2_MINSIZE            0x0600
		
		#define NO_RESPONSE                      0
		
		#define DEFAULT_TTL                      128
		
		#define MAC_COMPARE(mac1, mac2)          (memcmp(mac1, mac2, sizeof(MAC_Address_t)) == 0)
		#define IP_COMPARE(ip1, ip2)             (memcmp(ip1, ip2, sizeof(IP_Address_t)) == 0)

	/* Type Defines: */
		typedef struct
		{
			uint8_t       FrameData[ETHERNET_FRAME_SIZE_MAX];
			uint16_t      FrameLength;
			bool          FrameInBuffer;			
		} Ethernet_Frame_Info_t;

		typedef struct
		{
			uint8_t       Octets[6];
		} MAC_Address_t;
		
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
		
		typedef struct
		{
			uint8_t       Octets[4];
		} IP_Address_t;

		typedef struct
		{
			unsigned int  HeaderLength   : 4;
			unsigned int  Version        : 4;
			unsigned int  TypeOfService  : 8;
			unsigned int  TotalLength    : 16;

			unsigned int  Identification : 16;
			unsigned int  FragmentOffset : 13;
			unsigned int  Flags          : 3;

			unsigned int  TTL            : 8;
			unsigned int  Protocol       : 8;
			unsigned int  HeaderChecksum : 16;
			
			IP_Address_t  SourceAddress;
			IP_Address_t  DestinationAddress;
		} Ethernet_IP_Header_t;
		
		typedef struct
		{
			uint8_t       Type;
			uint8_t       Code;
			uint16_t      Checksum;
			uint16_t      Id;
			uint16_t      Sequence;
		} Ethernet_ICMP_Header_t;
		
		typedef struct
		{
			uint16_t      HardwareType;
			uint16_t      ProtocolType;
			
			uint8_t       HLEN;
			uint8_t       PLEN;
			uint16_t      Operation;
			
			MAC_Address_t SHA;
			IP_Address_t  SPA;
			MAC_Address_t THA;
			IP_Address_t  TPA;
		} Ethernet_ARP_Header_t;
		
		typedef struct
		{
		
		} Ethernet_TCP_Header_t;
		
	/* External Variables: */
		extern Ethernet_Frame_Info_t FrameIN;
		extern Ethernet_Frame_Info_t FrameOUT;

		extern MAC_Address_t ServerMACAddress;
		extern IP_Address_t  ServerIPAddress;
		extern MAC_Address_t NullMACAddress;
		extern MAC_Address_t BroadcastMACAddress;
		
	/* Function Prototypes: */
		void Ethernet_ProcessPacket(void);
		
		#if defined(INCLUDE_FROM_ETHERNET_C)
			static uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes);
			static uint16_t Ethernet_ProcessARPPacket(void* InDataStart, void* OutDataStart);
			static uint16_t Ethernet_ProcessIPPacket(void* InDataStart, void* OutDataStart);
			static uint16_t Ethernet_ProcessICMPPacket(void* InDataStart, void* OutDataStart);
			static uint16_t Ethernet_ProcessTCPPacket(void* InDataStart, void* OutDataStart);
		#endif

#endif
