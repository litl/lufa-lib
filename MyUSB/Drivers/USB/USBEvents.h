#ifndef USBEVENTS_H
#define USBEVENTS_H

	/* Private Defines */
		#define NO_EVENT_HOOK
	
	/* Event Hooks */
		#ifndef USB_EVENT_OnVBUSDetect
		  #define USB_EVENT_OnVBUSDetect()    NO_EVENT_HOOK
		#endif

		#ifndef USB_EVENT_OnUSBConnect
		  #define USB_EVENT_OnUSBConnect()    NO_EVENT_HOOK
		#endif

		#ifndef USB_EVENT_OnUSBDisconnect
		  #define USB_EVENT_OnUSBDisconnect() NO_EVENT_HOOK
		#endif

#endif
