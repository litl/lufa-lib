/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _ARP_H_
#define _ARP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		
		#include <MyUSB/Scheduler/Scheduler.h>
		
		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		#include "ProtocolDecoders.h"
		
	/* Type Defines: */	
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
		} ARP_Header_t;
	
	/* Tasks: */
		TASK(ARP_Task);
		
	/* Function Prototypes: */
		uint16_t ARP_ProcessARPPacket(void* InDataStart, void* OutDataStart);

#endif
