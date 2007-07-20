#ifndef USERCHAPTER9_H
#define USERCHAPTER9_H

	/* Includes: */
		#include <stdbool.h>
		
		#include "../../../Drivers/USB/LowLevel/Chapter9.h"

	/* Function Prototypes: */
		void USB_User_ProcessControlPacket(const uint8_t Request, const uint8_t RequestType);

#endif
