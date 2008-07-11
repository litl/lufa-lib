/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _IP_H_
#define _IP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		
		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"
	
	/* Macros: */
		#define SERVER_IP_ADDRESS                {10, 0, 0, 2}

		#define DEFAULT_TTL                      128
		
		#define IP_COMPARE(ip1, ip2)             (memcmp(ip1, ip2, sizeof(IP_Address_t)) == 0)
		
	/* Type Defines: */
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
		} IP_Header_t;
		
	/* Function Prototypes: */
		int16_t IP_ProcessIPPacket(void* InDataStart, void* OutDataStart);

#endif
