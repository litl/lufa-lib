/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Telnet.h"

char TelnetWelcome[] PROGMEM = "****************************\r\n"
                               "* Welcome to your USB AVR! *\r\n"
							   "****************************\r\n\r\n";
char TelnetCommand[] PROGMEM = "Command > ";

void Telnet_Init(void)
{
	/* Open the TELNET port in the TCP protocol so that TELNET connections to the device can be established */
	TCP_SetPortState(TCP_PORT_TELNET, TCP_Port_Open, Telnet_HandleRequest);
}

void Telnet_HandleRequest(TCP_ConnectionBuffer_t* Buffer)
{
	bool IsCommand = (Buffer->Data[0] == 0xFF);

	if (strncmp((char*)Buffer->Data, "LED1", 4) == 0)
	  LEDs_SetAllLEDs(LEDS_LED1);
	else if (strncmp((char*)Buffer->Data, "LED2", 4) == 0)
	  LEDs_SetAllLEDs(LEDS_LED2);
	else if (strncmp((char*)Buffer->Data, "LED3", 4) == 0)
	  LEDs_SetAllLEDs(LEDS_LED3);
	else if (strncmp((char*)Buffer->Data, "LED4", 4) == 0)
	  LEDs_SetAllLEDs(LEDS_LED4);
	
	if (IsCommand)
	{
		strcpy_P((char*)Buffer->Data, TelnetWelcome);
		strcpy_P((char*)&Buffer->Data[strlen((char*)Buffer->Data)], TelnetCommand);
		Buffer->Length = strlen((char*)Buffer->Data);
	}
	else if (strncmp((char*)Buffer->Data, "\r\n", 2) == 0)
	{
		strcpy_P((char*)Buffer->Data, TelnetCommand);
		Buffer->Length = strlen((char*)Buffer->Data);
	}
	else
	{
		Buffer->Ready = false;
	}
}