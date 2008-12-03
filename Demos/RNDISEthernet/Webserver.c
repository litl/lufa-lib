/*
             LUFA Library
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

#include "Webserver.h"

char PROGMEM HTTPHeader[] = "HTTP/1.1 200 OK\r\n"
                            "Server: LUFA RNDIS\r\n"
                            "Content-type: text/html\r\n"
                            "Connection: close\r\n\r\n";
char PROGMEM HTTPPage[]   = 
		"<html>"
		"	<head>"
		"		<title>"
		"			LUFA Webserver Demo"
		"		</title>"
		"	</head>"
		"	<body>"
		"		<h1>Hello from your USB AVR!</h1>"
		"		<p>"
		"			Hello! Welcome to the LUFA RNDIS Demo Webserver test page, running on your USB AVR via the LUFA library. This demonstrates the HTTP webserver, TCP/IP stack and RNDIS demo all running atop the LUFA USB stack."
		"			<br /><br />"
		"			<small>Project Information: <a href=\"http://www.fourwalledcubicle.com/LUFA.php\">http://www.fourwalledcubicle.com/LUFA.php</a>.</small>"
		"			<hr />"
		"			<i>LUFA Version: </i>" LUFA_VERSION_STRING
		"		</p>"
		"	</body>"
		"</html>";

void Webserver_Init(void)
{
	/* Open the HTTP port in the TCP protocol so that HTTP connections to the device can be established */
	TCP_SetPortState(TCP_PORT_HTTP, TCP_Port_Open, Webserver_ApplicationCallback);
}

static bool IsHTTPCommand(uint8_t* RequestHeader, char* Command)
{
	/* Returns true if the non null terminated string in RequestHeader matches the null terminated string Command */
	return (strncmp((char*)RequestHeader, Command, strlen(Command)) == 0);
}

void Webserver_ApplicationCallback(TCP_ConnectionState_t* ConnectionState, TCP_ConnectionBuffer_t* Buffer)
{
	char*          BufferDataStr = (char*)Buffer->Data;
	static uint8_t PageBlock     = 0;
	
	/* Check to see if a packet has been received on the TELNET port from a remote host */
	if (TCP_APP_HAS_RECEIVED_PACKET(Buffer))
	{
		if (IsHTTPCommand(Buffer->Data, "GET"))
		{
			PageBlock = 0;

			/* Copy the HTTP response header into the packet buffer */
			strcpy_P(BufferDataStr, HTTPHeader);
			
			/* Send the buffer contents to the host */
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));

			/* Lock the buffer to Device->Host transmissions only while we send the page contents */
			TCP_APP_CAPTURE_BUFFER(Buffer);
		}
		else if (IsHTTPCommand(Buffer->Data, "HEAD"))
		{
			/* Copy the HTTP response header into the packet buffer */
			strcpy_P(BufferDataStr, HTTPHeader);

			/* Send the buffer contents to the host */
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsHTTPCommand(Buffer->Data, "TRACE"))
		{
			/* Echo the host's query back to the host */
			TCP_APP_SEND_BUFFER(Buffer, Buffer->Length);
		}
		else
		{
			/* Unknown request, just clear the buffer (drop the packet) */
			TCP_APP_CLEAR_BUFFER(Buffer);
		}
	}
	else if (TCP_APP_HAVE_CAPTURED_BUFFER(Buffer))
	{
		uint16_t RemLength = strlen_P(&HTTPPage[PageBlock * HTTP_REPLY_BLOCK_SIZE]);
		uint16_t Length;
	
		/* Determine the length of the loaded block */
		Length = ((RemLength > HTTP_REPLY_BLOCK_SIZE) ? HTTP_REPLY_BLOCK_SIZE : RemLength);

		/* Copy the next buffer sized block of the page to the packet buffer */
		strncpy_P(BufferDataStr, &HTTPPage[PageBlock * HTTP_REPLY_BLOCK_SIZE], Length);
		
		/* Send the buffer contents to the host */
		TCP_APP_SEND_BUFFER(Buffer, Length);

		/* Check to see if the entire page has been sent */
		if (PageBlock++ == (sizeof(HTTPPage) / HTTP_REPLY_BLOCK_SIZE))
		{
			/* Unlock the buffer so that the host can fill it with future packets */
			TCP_APP_RELEASE_BUFFER(Buffer);
			
			/* Close the connection to the host */
			TCP_APP_CLOSECONNECTION(ConnectionState);
		}
	}
}
