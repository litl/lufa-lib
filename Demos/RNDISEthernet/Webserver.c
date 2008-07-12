/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Webserver.h"

char HTTPHeader[] PROGMEM = "HTTP/1.1 200 OK\r\nServer: MyUSB RNDIS\r\nContent-type: text/html\r\nConnection: close\r\n\r\n";
char HTTPPage[]   PROGMEM = 
		"<html>"
		"	<head>"
		"		<title>"
		"			MyUSB Webserver Demo"
		"		</title>"
		"	</head>"
		"	<body>"
		"		<h1>Hello from your USB AVR!</h1>"
		"		<p>"
		"			Hello <b>%REMOTEIP%</b>! Welcome to the MyUSB RNDIS Demo Webserver test page, running on port <b>%LOCALPORT%</b>. This demonstrates the HTTP webserver, TCP/IP stack and RNDIS demo all running atop the MyUSB USB stack."
		"			<br /><br />"
		"			<small>Project Information: <a href=\"http://www.fourwalledcubicle.com/MyUSB.php\">http://www.fourwalledcubicle.com/MyUSB.php</a>.</small>"
		"			<hr />"
		"			<i>MyUSB Version: </i>" MYUSB_VERSION_STRING
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
	return (strncmp((char*)RequestHeader, Command, strlen(Command)) == 0);
}

void Webserver_ApplicationCallback(TCP_ConnectionBuffer_t* Buffer)
{
	char*          BufferDataStr = (char*)Buffer->Data;
	static uint8_t PageBlock     = 0;
	
	if (TCP_APP_HAS_RECEIVED_PACKET(Buffer))
	{
		if (IsHTTPCommand(Buffer->Data, "GET"))
		{
			PageBlock = 0;

			strcpy_P(BufferDataStr, HTTPHeader);
			
			printf("Buffer captured.\r\n");
			
			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
			TCP_APP_CAPTURE_BUFFER(Buffer);
		}
		else if (IsHTTPCommand(Buffer->Data, "HEAD"))
		{
			strcpy_P(BufferDataStr, HTTPHeader);

			TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
		}
		else if (IsHTTPCommand(Buffer->Data, "TRACE"))
		{
			TCP_APP_SEND_BUFFER(Buffer, Buffer->Length);
		}
		else
		{
			TCP_APP_CLEAR_BUFFER(Buffer);
		}
	}
	else if (TCP_APP_HAVE_CAPTURED_BUFFER(Buffer))
	{
		uint16_t Length;
		uint16_t RemLength = strlen_P(&HTTPPage[PageBlock * HTTP_REPLY_BLOCK_SIZE]);
	
		printf("Next segment.\r\n");

		strncpy_P(BufferDataStr, &HTTPPage[PageBlock * HTTP_REPLY_BLOCK_SIZE], HTTP_REPLY_BLOCK_SIZE);
		Length = ((RemLength > HTTP_REPLY_BLOCK_SIZE) ? HTTP_REPLY_BLOCK_SIZE : RemLength);
		
		if (PageBlock++ == (sizeof(HTTPPage) / HTTP_REPLY_BLOCK_SIZE))
		  TCP_APP_RELEASE_BUFFER(Buffer);
		
		TCP_APP_SEND_BUFFER(Buffer, Length);
	}
}
