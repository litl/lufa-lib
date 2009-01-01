/*
             LUFA Library
     Copyright (C) Dean Camera, 2009.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2009  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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

#ifndef _DHCP_H_
#define _DHCP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
	
		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"

	/* Macros: */
		#define DHCP_OP_BOOTREQUEST       0x01
		#define DHCP_OP_BOOTREPLY         0x02
		
		#define DHCP_HTYPE_ETHERNET       0x01
		
		#define DHCP_MAGIC_COOKIE         0x63825363
		
		#define DHCP_OPTION_SUBNETMASK    1
		#define DHCP_OPTION_MESSAGETYPE   53
		#define DHCP_OPTION_DHCPSERVER    54
		#define DHCP_OPTION_PAD           0
		#define DHCP_OPTION_END           255
			
		#define DHCP_MESSAGETYPE_DISCOVER 1
		#define DHCP_MESSAGETYPE_OFFER    2
		#define DHCP_MESSAGETYPE_REQUEST  3
		#define DHCP_MESSAGETYPE_DECLINE  4
		#define DHCP_MESSAGETYPE_ACK      5
		#define DHCP_MESSAGETYPE_NACK     6
		#define DHCP_MESSAGETYPE_RELEASE  7

	/* Type Defines: */
	typedef struct
	{
		uint8_t  Operation;
		uint8_t  HardwareType;
		uint8_t  HardwareAddressLength;
		uint8_t  Hops;

		uint32_t TransactionID;

		uint16_t ElapsedSeconds;
		uint16_t Flags;
		
		IP_Address_t ClientIP;
		IP_Address_t YourIP;
		IP_Address_t NextServerIP;
		IP_Address_t RelayAgentIP;
		
		uint8_t ClientHardwareAddress[16];
		uint8_t ServerHostnameString[64];
		uint8_t BootFileName[128];
		
		uint32_t Cookie;
	} DHCP_Header_t;

	/* Function Prototypes: */
		int16_t DHCP_ProcessDHCPPacket(void* IPHeaderInStart, void* DHCPHeaderInStart, void* DHCPHeaderOutStart);

#endif
