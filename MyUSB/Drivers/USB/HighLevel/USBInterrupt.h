#ifndef USBINT_H
#define USBINT_H

	/* Includes */
		#include <avr/io.h>
		
		#include "../LowLevel/LowLevel.h"
		#include "../../../Configuration/USB/EventHooks.h"

	/* Private Macros */
		#define USB_INT_GETITEM1(a, b, c, d) a
		#define USB_INT_GETITEM2(a, b, c, d) b
		#define USB_INT_GETITEM3(a, b, c, d) c
		#define USB_INT_GETITEM4(a, b, c, d) d

		#define USB_INT_ENABLE(int)          USB_INT_GETITEM1(int) |= USB_INT_GETITEM2(int)
		#define USB_INT_DISABLE(int)         USB_INT_GETITEM1(int) &= ~(USB_INT_GETITEM2(int))
		#define USB_INT_ISENABLED(int)       USB_INT_GETITEM1(int) &  USB_INT_GETITEM2(int)
		#define USB_INT_OCCURED(int)         USB_INT_GETITEM3(int) &  USB_INT_GETITEM4(int)
		#define USB_INT_CLEAR(int)           USB_INT_GETITEM3(int) &= ~(USB_INT_GETITEM4(int))
	
		#define USB_INT_VBUS                 USBCON, (1 << VBUSTE) , USBINT, (1 << VBUSTI)
		#define USB_INT_WAKEUP               UDIEN , (1 << WAKEUPE), UDINT , (1 << WAKEUPI)
		#define USB_INT_SUSPEND              UDIEN , (1 << SUSPE)  , UDINT , (1 << SUSPI)
		#define USB_INT_EORSTI               UDIEN , (1 << EORSTE) , UDINT , (1 << EORSTI)

#endif
