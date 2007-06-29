#ifndef USBLOWLEVEL_H
#define USBLOWLEVEL_H

	/* Includes */
		#include <avr/io.h>
		#include <avr/interrupt.h>

		#include "../USB.h"

	/* Public Macros */
		#define USB_Get_VBUS_Status() (USBSTA & (1 << VBUS))

	/* Inline Functions */
		static inline void USB_Init(void)
		{
			UHWCON = (1 << UIMOD);
			USBCON = (1 << USBE);
		}

#endif
