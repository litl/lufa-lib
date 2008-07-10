/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _ICMP_H_
#define _ICMP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		
		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"
	
	/* Type Defines: */
		typedef struct
		{
			uint8_t       Type;
			uint8_t       Code;
			uint16_t      Checksum;
			uint16_t      Id;
			uint16_t      Sequence;
		} ICMP_Header_t;
		
	/* Function Prototypes: */
		uint16_t ICMP_ProcessICMPPacket(void* InDataStart, void* OutDataStart);

#endif
