#ifndef USERCHAPTER9_H
#define USERCHAPTER9_H

	/* Includes: */
		#include <stdbool.h>
		
		#include "../../../Scheduler/Scheduler.h"
		#include "../../../Drivers/USB/USB.h"

		#include "../../../Drivers/USBKEY/Joystick.h"

	/* Function Prototypes: */
		void USB_User_ProcessControlPacket(const uint8_t Request, const uint8_t RequestType);
		void USB_User_CreateEndpoints(void);

#endif
