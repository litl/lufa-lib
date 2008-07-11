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
							   "****************************\r\n"
							   "\r\n"
							   "Type HELP for a list of commands.\r\n\r\n\r\n";

char TelnetCommand[] PROGMEM = "Command > ";

char TelnetHelp[]    PROGMEM = "\r\nCommand List:\r\n"
                               " > HELP    - Displays this message\r\n"
                               " > LEDONx  - Turns LED number \"x\" on\r\n"
                               " > LEDOFFx - Turns LED number \"x\" off\r\n\r\n";

void Telnet_Init(void)
{
	/* Open the TELNET port in the TCP protocol so that TELNET connections to the device can be established */
	TCP_SetPortState(TCP_PORT_TELNET, TCP_Port_Open, Telnet_HandleRequest);
}

void Telnet_HandleRequest(TCP_ConnectionBuffer_t* Buffer)
{
	Buffer->Data[Buffer->Length] = 0x00;

	if (Buffer->Data[0] == 0xFF)
	{
		strcpy_P((char*)Buffer->Data, TelnetWelcome);
		strcpy_P((char*)&Buffer->Data[strlen_P(TelnetWelcome)], TelnetCommand);
		Buffer->Ready = true;
	}
	else if (strcmp((char*)Buffer->Data, "\r\n") == 0)
	{
		strcpy_P((char*)Buffer->Data, TelnetCommand);
		Buffer->Ready = true;
	}
	else if (strcmp((char*)Buffer->Data, "HELP") == 0)
	{
		strcpy_P((char*)Buffer->Data, TelnetHelp);
		Buffer->Ready = true;
	}
	else if (strncmp((char*)Buffer->Data, "LEDON", (sizeof("LEDON") - 1)) == 0)
	{
		switch (Buffer->Data[sizeof("LEDON") - 1])
		{
			case '1':
				LEDs_TurnOnLEDs(LEDS_LED1);
				break;
			case '2':
				LEDs_TurnOnLEDs(LEDS_LED2);
				break;
			case '3':
				LEDs_TurnOnLEDs(LEDS_LED3);
				break;
			case '4':
				LEDs_TurnOnLEDs(LEDS_LED4);
				break;
		}

		Buffer->Ready = false;
	}
	else if (strncmp((char*)Buffer->Data, "LEDOFF", (sizeof("LEDOFF") - 1)) == 0)
	{
		switch (Buffer->Data[sizeof("LEDOFF") - 1])
		{
			case '1':
				LEDs_TurnOffLEDs(LEDS_LED1);
				break;
			case '2':
				LEDs_TurnOffLEDs(LEDS_LED2);
				break;
			case '3':
				LEDs_TurnOffLEDs(LEDS_LED3);
				break;
			case '4':
				LEDs_TurnOffLEDs(LEDS_LED4);
				break;
		}

		Buffer->Ready = false;
	}

	Buffer->Length = strlen((char*)Buffer->Data);
}