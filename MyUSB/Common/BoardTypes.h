/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#if !defined(INCLUDE_FROM_BOARD_DRIVER)
	#error Do not include this file directly. It will be included as needed by library headers.
#endif
	
#if !defined(__BOARDTYPES_H__) && defined(INCLUDE_FROM_BOARD_DRIVER)
#define __BOARDTYPES_H__

	/* Macros: */
		#define BOARD_USBKEY                 0
		#define BOARD_STK525                 1
		#define BOARD_STK526                 2

#endif
