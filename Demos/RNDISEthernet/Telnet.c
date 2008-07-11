/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Telnet.h"

char TelnetWelcome[]     PROGMEM = "****************************\r\n"
                                   "* Welcome to your USB AVR! *\r\n"
							       "****************************\r\n"
							       "\r\n"
							       "Type HELP for a list of commands.\r\n\r\n\r\n";

char TelnetCommand[]     PROGMEM = "Command > ";
char TelnetCommandOK[]   PROGMEM = " Command completed sucessfully.\r\n\r\n";
char TelnetCommandFail[] PROGMEM = " Command failed.\r\n\r\n";
char TelnetCommandUnk[]  PROGMEM = " Unknown command. Type HELP for a list of commands.\r\n\r\n";

char TelnetHelp[]        PROGMEM = "\r\nCommand List:\r\n"
                                   " > HELP    - Displays this message\r\n"
                                   " > LEDONx  - Turns LED number \"x\" on\r\n"
                                   " > LEDOFFx - Turns LED number \"x\" off\r\n\r\n";

void Telnet_Init(void)
{
	/* Open the TELNET port in the TCP protocol so that TELNET connections to the device can be established */
	TCP_SetPortState(TCP_PORT_TELNET, TCP_Port_Open, Telnet_ApplicationCallback);
}

static bool IsTELNETCommand(uint8_t* Data, char* Command)
{
	return (strncmp((char*)Data, Command, strlen(Command)) == 0);
}

void Telnet_ApplicationCallback(TCP_ConnectionBuffer_t* Buffer)
{
	char* BufferDataStr = (char*)Buffer->Data;

	if (TCP_APP_HAS_RECEIVED_PACKET(Buffer))
	{
		if (Buffer->Data[0] == 0xFF)
		{
			strcpy_P(BufferDataStr, TelnetWelcome);
			strcpy_P(&BufferDataStr[strlen_P(TelnetWelcome)], TelnetCommand);
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsTELNETCommand(Buffer->Data, "\r\n"))
		{
			strcpy_P(BufferDataStr, TelnetCommand);
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsTELNETCommand(Buffer->Data, "HELP"))
		{
			strcpy_P(BufferDataStr, TelnetHelp);
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsTELNETCommand(Buffer->Data, "LEDON"))
		{
			bool CommandOK = true;

			switch (Buffer->Data[strlen("LEDON")])
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
				default:
					CommandOK = false;
					break;
			}

			if (CommandOK)
			  strcpy_P(BufferDataStr, TelnetCommandOK);
			else
			  strcpy_P(BufferDataStr, TelnetCommandFail);
			
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsTELNETCommand(Buffer->Data, "LEDOFF"))
		{
			bool CommandOK = true;
		
			switch (Buffer->Data[strlen("LEDOFF")])
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
				default:
					CommandOK = false;
					break;
			}
			
			if (CommandOK)
			  strcpy_P(BufferDataStr, TelnetCommandOK);
			else
			  strcpy_P(BufferDataStr, TelnetCommandFail);
			
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else
		{
			strcpy_P(BufferDataStr, TelnetCommandUnk);
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
	}
}