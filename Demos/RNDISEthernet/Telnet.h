/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _TELNET_H_
#define _TELNET_H_

	/* Includes: */
		#include <avr/io.h>
		#include <string.h>
		
		#include <MyUSB/Drivers/Board/LEDs.h>
		
		#include "TCP.h"
	
	/* Function Prototypes: */
		void Telnet_Init(void);
		void Telnet_ApplicationCallback(TCP_ConnectionBuffer_t* Buffer);

#endif
