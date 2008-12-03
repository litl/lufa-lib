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

#ifndef _ETHERNET_PROTOCOLS_H_
#define _ETHERNET_PROTOCOLS_H_

	/* Macros: */
		#define ETHERTYPE_IPV4                   0x0800
		#define ETHERTYPE_ARP                    0x0806
		#define ETHERTYPE_RARP                   0x8035
		#define ETHERTYPE_APPLETALK              0x809b
		#define ETHERTYPE_APPLETALKARP           0x80f3
		#define ETHERTYPE_IEEE8021Q              0x8100
		#define ETHERTYPE_NOVELLIPX              0x8137
		#define ETHERTYPE_NOVELL                 0x8138
		#define ETHERTYPE_IPV6                   0x86DD
		#define ETHERTYPE_COBRANET               0x8819
		#define ETHERTYPE_PROVIDERBRIDGING       0x88a8
		#define ETHERTYPE_MPLSUNICAST            0x8847
		#define ETHERTYPE_MPLSMULTICAST          0x8848
		#define ETHERTYPE_PPPoEDISCOVERY         0x8863
		#define ETHERTYPE_PPPoESESSION           0x8864
		#define ETHERTYPE_EAPOVERLAN             0x888E
		#define ETHERTYPE_HYPERSCSI              0x889A
		#define ETHERTYPE_ATAOVERETHERNET        0x88A2
		#define ETHERTYPE_ETHERCAT               0x88A4
		#define ETHERTYPE_SERCOSIII              0x88CD
		#define ETHERTYPE_CESoE                  0x88D8
		#define ETHERTYPE_MACSECURITY            0x88E5
		#define ETHERTYPE_FIBRECHANNEL           0x8906
		#define ETHERTYPE_QINQ                   0x9100
		#define ETHERTYPE_VLLT                   0xCAFE
		
		#define PROTOCOL_ICMP                    1
		#define PROTOCOL_IGMP                    2
		#define PROTOCOL_TCP                     6
		#define PROTOCOL_UDP                     17
		#define PROTOCOL_OSPF                    89
		#define PROTOCOL_SCTP                    132
		
		#define ICMP_TYPE_ECHOREPLY              0
		#define ICMP_TYPE_DESTINATIONUNREACHABLE 3
		#define ICMP_TYPE_SOURCEQUENCH           4
		#define ICMP_TYPE_REDIRECTMESSAGE        5
		#define ICMP_TYPE_ECHOREQUEST            8
		#define ICMP_TYPE_TIMEEXCEEDED           11
		
		#define ICMP_ECHOREPLY_ECHOREPLY         0
		#define ICMP_ECHOREQUEST_ECHOREQUEST     0
		#define ICMP_DESTUNREACHABLE_NETWORK     0
		#define ICMP_DESTUNREACHABLE_HOST        1
		
		#define ARP_OPERATION_REQUEST            1
		#define ARP_OPERATION_REPLY              2

	/* Type Defines: */
		typedef struct
		{
			uint8_t       Octets[6];
		} MAC_Address_t;
		
		typedef struct
		{
			uint8_t       Octets[4];
		} IP_Address_t;

#endif
