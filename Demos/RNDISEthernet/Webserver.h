/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		
		#include <MyUSB/Version.h>
		
		#include "TCP.h"
	
	/* Function Prototypes: */
		void Webserver_Init(void);
		void Webserver_ApplicationCallback(TCP_ConnectionBuffer_t* Buffer);

#endif
