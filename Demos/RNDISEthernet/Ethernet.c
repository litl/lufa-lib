/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Ethernet.h"

/* Global Variables: */
Ethernet_Frame_Info_t FrameIN;
Ethernet_Frame_Info_t FrameOUT;

void Ethernet_ProcessPacket(void)
{
	/* Cast the incomming Ethernet frame to the Ethernet header type */
	Ethernet_Frame_Header_t* FrameINHeader = (Ethernet_Frame_Header_t*)&FrameIN.FrameData;

	/* Print out the total length of the frame */
	printf("Frame RX Size: %d\r\n", FrameIN.FrameLength);
	
	/* Print out the formatted MAC source address of the frame */
	printf("Source: %02x:%02x:%02x:%02x:%02x:%02x\r\n", FrameINHeader->Source.Octets[0],
	                                                    FrameINHeader->Source.Octets[1],
	                                                    FrameINHeader->Source.Octets[2],
	                                                    FrameINHeader->Source.Octets[3],
	                                                    FrameINHeader->Source.Octets[4],
	                                                    FrameINHeader->Source.Octets[5]);

	/* Print out the formatted MAC destination address of the frame */
	printf("Dest: %02x:%02x:%02x:%02x:%02x:%02x\r\n",   FrameINHeader->Destination.Octets[0],
	                                                    FrameINHeader->Destination.Octets[1],
	                                                    FrameINHeader->Destination.Octets[2],
	                                                    FrameINHeader->Destination.Octets[3],
	                                                    FrameINHeader->Destination.Octets[4],
	                                                    FrameINHeader->Destination.Octets[5]);
	
	/* If the frame is an Ethernet II frame, print out the Ethertype (protocol) field */
	if (SwapEndian_16(FrameIN.FrameLength) > ETHERNET_VER2_MINSIZE)
	  printf("Protocol: 0x%04x\r\n", SwapEndian_16(FrameINHeader->EtherType));
	else
	  printf("Protocol: UNKNOWN E1\r\n");
			
	printf("\r\n");
}
