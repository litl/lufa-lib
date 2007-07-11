#ifndef USBDEVICE_H
#define USBDEVICE_H

	/* Private Macros */
		#define USB_DEV_Detach()       UDCON |=  (1 << DETACH)
		#define USB_DEV_Attach()       UDCON &= ~(1 << DETACH)
		
		#define USB_SPD_Low()          UDCON |= (1 << LSM)
		#define USB_SPD_High()		   UDCON &= ~(1 << LSM)

#endif
