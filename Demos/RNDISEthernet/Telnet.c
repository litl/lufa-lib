/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Telnet.h"

void Telnet_Init(void)
{
	/* Open the TELNET port in the TCP protocol so that TELNET connections to the device can be established */
	TCP_SetPortState(TCP_PORT_TELNET, TCP_Port_Open, Telnet_HandleRequest);
}

void Telnet_HandleRequest(TCP_ConnectionBuffer_t* Buffer)
{
	printf("Telnet Data Len %u.\r\n", Buffer->Length);
		
	for (uint8_t C = 0; C < Buffer->Length; C++)
	 printf("%c", Buffer->Data[C]);
		 
	printf("\r\n+END\r\n");

	if (strncmp((char*)Buffer->Data, "\r\n", sizeof("\r\n")))
	{
		if (strncmp((char*)Buffer->Data, "LED1", sizeof("LED1")) == 0)
		{
			LEDs_SetAllLEDs(LEDS_LED1);
			printf("LED1\r\n");
		}
		else
		{
			LEDs_SetAllLEDs(LEDS_LED2);
			printf("UNK\r\n");
		}
	}
	
	Buffer->Ready = false;
}