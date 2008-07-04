/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Ethernet.h"

/* Global Variables: */
uint8_t  EthernetFrameIN[ETHERNET_FRAME_SIZE];
uint8_t  EthernetFrameOUT[ETHERNET_FRAME_SIZE];

uint16_t EthernetFrameINLength;
uint16_t EthernetFrameOUTLength;

bool     IsFrameIN  = false;
bool     IsFrameOUT = false;


void Ethernet_ProcessPacket(void)
{
	Ethernet_Frame_Header_t* EthernetFrame = (Ethernet_Frame_Header_t*)EthernetFrameIN;

	printf("Frame RX Size: %d\r\n", EthernetFrameINLength);
	
	printf("Source: %02x:%02x:%02x:%02x:%02x:%02x\r\n", EthernetFrame->Source.Octets[0],
	                                                    EthernetFrame->Source.Octets[1],
	                                                    EthernetFrame->Source.Octets[2],
	                                                    EthernetFrame->Source.Octets[3],
	                                                    EthernetFrame->Source.Octets[4],
	                                                    EthernetFrame->Source.Octets[5]);

	printf("Dest: %02x:%02x:%02x:%02x:%02x:%02x\r\n",   EthernetFrame->Destination.Octets[0],
	                                                    EthernetFrame->Destination.Octets[1],
	                                                    EthernetFrame->Destination.Octets[2],
	                                                    EthernetFrame->Destination.Octets[3],
	                                                    EthernetFrame->Destination.Octets[4],
	                                                    EthernetFrame->Destination.Octets[5]);
	
	if (SwapEndian_16(EthernetFrame->Length) > ETHERNET_VER2_MINSIZE)
	  printf("Protocol: 0x%04x\r\n", SwapEndian_16(EthernetFrame->EtherType));
	else
	  printf("Protocol: UNKNOWN E1\r\n");
			
	printf("\r\n");
}
