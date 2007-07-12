#ifndef USBTASK_H
#define USBTASK_H

	#include <avr/io.h>
	#include <stdbool.h>
	
	#include "../../../Common/Scheduler.h"
	#include "../LowLevel/LowLevel.h"

	extern bool USBConnected;
	extern bool USBInitialized;

	TASK(USB_USBTask);

	void USB_InitTaskPointer(void);
	void USB_DeviceTask(void);
	void USB_HostTask(void);

#endif
