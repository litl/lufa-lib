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

#ifndef _DHCP_H_
#define _DHCP_H_

	/* Includes: */
		#include <avr/io.h>
	
		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"
			
	/* Type Defines: */
	typedef struct
	{
		uint8_t  Operation;
		uint8_t  HardwareAddressType;
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

		uint8_t Options[];
	} DHCP_Header_t;

	/* Function Prototypes: */
		int16_t DHCP_ProcessDHCPPacket(void* DHCPHeaderInStart, void* DHCPHeaderOutStart);

#endif
