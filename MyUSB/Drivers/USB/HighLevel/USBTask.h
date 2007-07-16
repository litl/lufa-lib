#ifndef USBTASK_H
#define USBTASK_H

	/* Includes */
		#include <avr/io.h>
		#include <stdbool.h>
		
		#include "../../../Scheduler/Scheduler.h"
		#include "../LowLevel/LowLevel.h"
	
	/* Private Macros */
		#define USB_IsSetupRecieved()    (UEINTX & (1 << RXSTPI))
		#define USB_ClearSetupRecieved() UEINTX &= ~(1 << RXSTPI)
	
	/* External Variables */
		extern bool USB_IsConnected;
		extern bool USB_IsInitialized;

	/* Function Prototypes */
		void USB_USBTask(void);
		void USB_InitTaskPointer(void);
		void USB_DeviceTask(void);
		void USB_HostTask(void);

#endif
