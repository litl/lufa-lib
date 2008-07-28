/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
	/* Returns true if the non null terminated string in Data matches the null terminated string Command */
	return (strncmp((char*)Data, Command, strlen(Command)) == 0);
}

void Telnet_ApplicationCallback(TCP_ConnectionBuffer_t* Buffer)
{
	char* BufferDataStr = (char*)Buffer->Data;

	/* Check to see if a packet has been received on the TELNET port from a remote host */
	if (TCP_APP_HAS_RECEIVED_PACKET(Buffer))
	{
		/* If first byte is 0xFF it is a control message, send back the welcome text and command prompt */
		if (Buffer->Data[0] == 0xFF)
		{
			/* Copy welcome and command line text to the packet buffer */
			strcpy_P(BufferDataStr, TelnetWelcome);
			strcpy_P(&BufferDataStr[strlen_P(TelnetWelcome)], TelnetCommand);
			
			/* Send the packet contents back to the host */
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsTELNETCommand(Buffer->Data, "\r\n"))
		{
			/* Copy the command line text to the packet buffer */
			strcpy_P(BufferDataStr, TelnetCommand);
			
			/* Send the packet contents back to the host */
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsTELNETCommand(Buffer->Data, "HELP"))
		{
			/* Copy the help text to the packet buffer */
			strcpy_P(BufferDataStr, TelnetHelp);

			/* Send the packet contents back to the host */
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsTELNETCommand(Buffer->Data, "LEDON"))
		{
			bool CommandOK = true;

			/* Turn on the LED number given in the command */
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
			
			/* Send the packet contents back to the host */
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsTELNETCommand(Buffer->Data, "LEDOFF"))
		{
			bool CommandOK = true;
		
			/* Turn off the LED number given in the command */
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
			
			/* Send the packet contents back to the host */
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else
		{
			/* Copy the unknown command response into the packet buffer */
			strcpy_P(BufferDataStr, TelnetCommandUnk);

			/* Send the packet contents back to the host */
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
	}
}