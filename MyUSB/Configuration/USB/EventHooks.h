#ifndef EVENTHOOKS_H
#define EVENTHOOKS_H

	/* Private Macros */
		#define NO_EVENT_HOOK                asm ("")

	/* Place any includes, defines and such used by your user-application hooks here. */
		#include "../../Drivers/USBKEY/Bicolour.h"

	/* Configure user-application hooks here. */
		#define USB_EVENT_OnVBUSChange()	 NO_EVENT_HOOK
		#define USB_EVENT_OnVBUSConnect()    NO_EVENT_HOOK
		#define USB_EVENT_OnVBUSDisconnect() NO_EVENT_HOOK
		#define USB_EVENT_OnUSBConnect()     { Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN); }
		#define USB_EVENT_OnUSBDisconnect()  { Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_ORANGE); }
		#define USB_EVENT_OnSuspend()        { Bicolour_SetLeds(BICOLOUR_ALL_LEDS); }
		#define USB_EVENT_OnWakeUp()         { Bicolour_SetLeds(BICOLOUR_LED1_GREEN | BICOLOUR_LED2_GREEN); }
		#define USB_EVENT_OnReset()          NO_EVENT_HOOK

#endif
