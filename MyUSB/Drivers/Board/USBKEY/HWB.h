/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Board specific HWB driver header for the USBKEY.
 *
 *  \note This file should not be included directly. It is automatically included as needed by the HWB driver
 *        dispatch header located in MyUSB/Drivers/Board/HWB.h.
 */
 
#ifndef __HWB_USBKEY_H__
#define __HWB_USBKEY_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../../Common/Common.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(INCLUDE_FROM_HWB_H)
			#error Do not include this file directly. Include MyUSB/Drivers/Board/HWB.h instead.
		#endif
		
	/* Public Interface - May be used in end-application: */
		/* Inline Functions: */
		#if !defined(__DOXYGEN__)
			static inline void HWB_Init(void)
			{
				DDRE  &= ~(1 << 2);
				PORTE |=  (1 << 2);
			}

			static inline bool HWB_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline bool HWB_GetStatus(void)
			{
				return (!(PINE & (1 << 2)));
			}
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
			
#endif
