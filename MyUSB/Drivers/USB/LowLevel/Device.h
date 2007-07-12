#ifndef USBDEVICE_H
#define USBDEVICE_H

	/* Public Macros */
		#define USB_DEV_LOWSPEED       (1 << 0)
		#define USB_DEV_HIGHSPEED      (0 << 0)

	/* Private Macros */
		#define USB_DEV_Detach()       UDCON |=  (1 << DETACH)
		#define USB_DEV_Attach()       UDCON &= ~(1 << DETACH)
		
		#define USB_DEV_SetLowSpeed()  UDCON |= (1 << LSM)
		#define USB_DEV_SetHighSpeed() UDCON &= ~(1 << LSM)

#endif
