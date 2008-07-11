/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Webserver.h"

char HTTPHeader[] PROGMEM = "HTTP/1.1 200 OK\r\nServer: MyUSB RNDIS\r\nContent-type: text/html\r\nConnection: close\r\n\r\n";
char HTTPPage[]   PROGMEM = "<html><head><title>MyUSB Webserver Test</title></head><body>Hello from the USBKEY!</body></html>\n";


void Webserver_Init(void)
{
	/* Open the HTTP port in the TCP protocol so that HTTP connections to the device can be established */
	TCP_SetPortState(TCP_PORT_HTTP, TCP_Port_Open, Webserver_HandleRequest);
}

void Webserver_HandleRequest(TCP_ConnectionBuffer_t* Buffer)
{
	strcpy_P((char*)Buffer->Data, HTTPHeader);
	strcpy_P((char*)&Buffer->Data[sizeof(HTTPHeader)], HTTPPage);
	
	Buffer->Length = (sizeof(HTTPHeader) + sizeof(HTTPPage) - 1);
}
