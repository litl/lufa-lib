#ifndef USBEVENTS_H
#define USBEVENTS_H

	/* Private Defines */
		#define NO_EVENT_HOOK                  asm ("")
	
	/* Event Hooks */
		#ifndef USB_EVENT_OnVBUSChange
		  #define USB_EVENT_OnVBUSChange()     NO_EVENT_HOOK
		#endif

		#ifndef USB_EVENT_OnVBUSConnect
		  #define USB_EVENT_OnVBUSConnect()    NO_EVENT_HOOK
		#endif
		
		#ifndef USB_EVENT_OnVBUSDisconnect
		  #define USB_EVENT_OnVBUSDisconnect() NO_EVENT_HOOK
		#endif
		
		#ifndef USB_EVENT_OnUSBConnect
		  #define USB_EVENT_OnUSBConnect()     NO_EVENT_HOOK
		#endif

		#ifndef USB_EVENT_OnUSBDisconnect
		  #define USB_EVENT_OnUSBDisconnect()  NO_EVENT_HOOK
		#endif

		#ifndef USB_EVENT_OnSuspend
		  #define USB_EVENT_OnSuspend()        NO_EVENT_HOOK
		#endif

		#ifndef USB_EVENT_OnWakeUp
		  #define USB_EVENT_OnWakeUp()         NO_EVENT_HOOK
		#endif

#endif
