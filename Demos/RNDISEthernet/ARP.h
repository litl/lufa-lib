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

#ifndef _ARP_H_
#define _ARP_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		
		#include <LUFA/Scheduler/Scheduler.h>
		
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
		
	/* Function Prototypes: */
		int16_t ARP_ProcessARPPacket(void* InDataStart, void* OutDataStart);

#endif
