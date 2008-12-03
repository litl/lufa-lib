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

#ifndef _IP_H_
#define _IP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		
		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"
	
	/* Macros: */
		#define NULL_IP_ADDRESS                  {0, 0, 0, 0}
		#define CLIENT_IP_ADDRESS                {10, 0, 0, 1}
		#define SERVER_IP_ADDRESS                {10, 0, 0, 2}
		#define BROADCAST_IP_ADDRESS             {0xFF, 0xFF, 0xFF, 0xFF}

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
