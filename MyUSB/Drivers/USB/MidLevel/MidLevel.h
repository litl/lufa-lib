#ifndef USBMIDLEVEL_H
#define USBMIDLEVEL_H

	/* Includes */
		#include <avr/io.h>

		#include "../USB.h"

	/* Inline Functions */
		static inline void USB_Init(const uint8_t Mode)
		{
			USB_LowLevelInit(Mode);
			USB_PowerOn();
		}

#endif
