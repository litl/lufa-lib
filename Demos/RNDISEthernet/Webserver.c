/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Webserver.h"

char HTTPHeader[] PROGMEM = "HTTP/1.1 200 OK\r\n\r\n";
char HTTPPage[]   PROGMEM = "<html><body>Hello from the USBKEY!</body></html>";


void Webserver_Init(void)
{
	/* Open the HTTP port in the TCP protocol so that HTTP connections to the device can be established */
	TCP_SetPortState(TCP_PORT_HTTP, TCP_Port_Open, Webserver_HandleRequest);
}

void Webserver_HandleRequest(TCP_ConnectionBuffer_t* Buffer)
{
	memcpy_P(Buffer->Data, HTTPHeader, sizeof(HTTPHeader));
	memcpy_P(&Buffer->Data[sizeof(HTTPHeader)], HTTPPage, sizeof(HTTPPage));
	
	Buffer->Length = (sizeof(HTTPHeader) + sizeof(HTTPPage));

	Buffer->Direction = TCP_PACKETDIR_OUT;
	Buffer->Ready = true;
}
