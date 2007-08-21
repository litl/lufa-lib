/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef USBINT_H
#define USBINT_H

	/* Includes */
		#include <avr/io.h>
		
		#include "../LowLevel/LowLevel.h"
		#include "../LowLevel/Endpoint.h"

	/* Private Macros */
		#define USB_INT_GET_EN_REG(a, b, c, d)  a
		#define USB_INT_GET_EN_MASK(a, b, c, d) b
		#define USB_INT_GET_INT_REG(a, b, c, d)  c
		#define USB_INT_GET_INT_MASK(a, b, c, d) d

		#define USB_INT_ENABLE(int)          USB_INT_GET_EN_REG(int)  |=   USB_INT_GET_EN_MASK(int)
		#define USB_INT_DISABLE(int)         USB_INT_GET_EN_REG(int)  &= ~(USB_INT_GET_EN_MASK(int))
		#define USB_INT_ISENABLED(int)       USB_INT_GET_EN_REG(int)  &    USB_INT_GET_EN_MASK(int)
		#define USB_INT_OCCURED(int)         USB_INT_GET_INT_REG(int) &    USB_INT_GET_INT_MASK(int)
		#define USB_INT_CLEAR(int)           USB_INT_GET_INT_REG(int) &= ~(USB_INT_GET_INT_MASK(int))
	
		#define USB_INT_VBUS                 USBCON, (1 << VBUSTE) , USBINT, (1 << VBUSTI)
		#define USB_INT_IDTI                 USBCON, (1 << IDTE)   , USBINT, (1 << IDTI)
		#define USB_INT_WAKEUP               UDIEN , (1 << WAKEUPE), UDINT , (1 << WAKEUPI)
		#define USB_INT_SUSPEND              UDIEN , (1 << SUSPE)  , UDINT , (1 << SUSPI)
		#define USB_INT_EORSTI               UDIEN , (1 << EORSTE) , UDINT , (1 << EORSTI)

	/* Function Prototypes */
		void USB_INT_DisableAllInterrupts(void);

#endif
