/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _PROTOCOL_DECODERS_H_
#define _PROTOCOL_DECODERS_H_

	/* Includes: */
		#include <avr/io.h>
		
		#include <MyUSB/Drivers/AT90USBXXX/Serial_Stream.h>
		
		#include "EthernetProtocols.h"
		#include "Ethernet.h"
		
	/* Function Prototypes: */
		void DecodeEthernetFrameHeader(void* InDataStart);
		void DecodeARPHeader(void* InDataStart);
		void DecodeIPHeader(void* InDataStart);
		void DecodeICMPHeader(void* InDataStart);
		void DecodeTCPHeader(void* InDataStart);

#endif
