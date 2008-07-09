/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "Webserver.h"

void Webserver_Init(void)
{
	TCP_StartListening(SwapEndian_16(TCP_PORT_HTTP));
}
