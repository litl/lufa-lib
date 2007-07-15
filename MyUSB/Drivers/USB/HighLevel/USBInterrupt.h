#ifndef USBINT_H
#define USBINT_H

	/* Includes */
		#include <avr/io.h>
		
		#include "../LowLevel/LowLevel.h"
		#include "../USBEvents.h"

	/* Private Macros */
		#define USB_INT_VBUS_Enable()      USBCON |= (1 << VBUSTE)
		#define USB_INT_VBUS_Disable()     USBCON &= ~(1 << VBUSTE)
		#define USB_INT_VBUS_IsEnabled()   (USBCON & (1 << VBUSTE)) 
		#define USB_INT_VBUS_IntOccured()  (USBINT & (1 << VBUSTI)) 
		#define USB_INT_VBUS_Reset()       USBINT &= ~(1 << VBUSTI)

#endif
