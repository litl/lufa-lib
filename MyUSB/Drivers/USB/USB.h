#ifndef MYUSB_H
#define MYUSB_H

	/* Includes */
		#include <stdbool.h>
	
		#include "LowLevel/LowLevel.h"		
		#include "../../Common/Scheduler.h"

	/* Task Definitions */
		TASK(USB_ManagementTask);
	
#endif
