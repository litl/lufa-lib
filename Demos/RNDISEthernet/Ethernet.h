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
		
		#include <MyUSB/Drivers/AT90USBXXX/Serial_Stream.h>
		
	/* Macros: */
		#define ETHERNET_FRAME_SIZE             1522
		
		#define ETHERNET_VER1_MAXSIZE           1500
		#define ETHERNET_VER2_MINSIZE           0x0600
		
		#define ETHERTYPE_IPV4                  0x0800
		#define ETHERTYPE_ARP                   0x0806
		#define ETHERTYPE_RARP                  0x8035
		#define ETHERTYPE_APPLETALK             0x809b
		#define ETHERTYPE_APPLETALKARP          0x80f3
		#define ETHERTYPE_IEEE8021Q             0x8100
		#define ETHERTYPE_NOVELLIPX             0x8137
		#define ETHERTYPE_NOVELL                0x8138
		#define ETHERTYPE_IPV6                  0x86DD
		#define ETHERTYPE_COBRANET              0x8819
		#define ETHERTYPE_PROVIDERBRIDGING      0x88a8
		#define ETHERTYPE_MPLSUNICAST           0x8847
		#define ETHERTYPE_MPLSMULTICAST         0x8848
		#define ETHERTYPE_PPPoEDISCOVERY        0x8863
		#define ETHERTYPE_PPPoESESSION          0x8864
		#define ETHERTYPE_EAPOVERLAN            0x888E
		#define ETHERTYPE_HYPERSCSI             0x889A
		#define ETHERTYPE_ATAOVERETHERNET       0x88A2
		#define ETHERTYPE_ETHERCAT              0x88A4
		#define ETHERTYPE_SERCOSIII             0x88CD
		#define ETHERTYPE_CESoE                 0x88D8
		#define ETHERTYPE_MACSECURITY           0x88E5
		#define ETHERTYPE_FIBRECHANNEL          0x8906
		#define ETHERTYPE_QINQ                  0x9100
		#define ETHERTYPE_VLLT                  0xCAFE
		
	/* Type Defines: */
		typedef struct
		{
			uint8_t Octets[6];
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
			
			uint8_t       Payload[];
		} Ethernet_Frame_Header_t;
		
	/* External Variables: */
		extern uint8_t  EthernetFrameIN[];
		extern uint8_t  EthernetFrameOUT[];

		extern uint16_t EthernetFrameINLength;
		extern uint16_t EthernetFrameOUTLength;

		extern bool     IsFrameIN;
		extern bool     IsFrameOUT;

	/* Function Prototypes: */
		void Ethernet_ProcessPacket(void);

#endif
