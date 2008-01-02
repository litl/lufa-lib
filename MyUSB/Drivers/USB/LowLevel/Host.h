/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __HOST_H__
#define __HOST_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../../Common/Common.h"
		#include "../HighLevel/USBInterrupt.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define USB_HOST_DEVICEADDRESS             1
			#define USB_HOST_TIMEOUT_MS                1000
			
			#define USB_HOST_ResetBus()                MACROS{ UHCON |=  (1 << RESET);          }MACROE
			#define USB_HOST_ResetBus_IsDone()               ((UHCON &   (1 << RESET)) ? false : true)
			
			#define USB_HOST_SOFGeneration_Enable()    MACROS{ UHCON |=  (1 << SOFEN);          }MACROE 
			#define USB_HOST_SOFGeneration_Disable()   MACROS{ UHCON &= ~(1 << SOFEN);          }MACROE 
			#define USB_HOST_SOFGeneration_IsEnabled()       ((UHCON &   (1 << SOFEN)) ? true : false)
			
			#define USB_HOST_IsRemoteWakeupSent()            ((UHINT &   (1 << RXRSMI)) ? true : false)
			#define USB_HOST_ClearRemoteWakeupSent()   MACROS{ UHINT &= ~(1 << RXRSMI);         }MACROE
			#define USB_HOST_SendResume()              MACROS{ UHCON |=  (1 << RESUME);         }MACROE
			
		/* Enums: */
			enum USB_Host_States
			{
				HOST_STATE_Unattached           = 0,
				HOST_STATE_Attached             = 1,
				HOST_STATE_Powered              = 2,
				HOST_STATE_Default              = 3,
				HOST_STATE_Addressed            = 4,
				HOST_STATE_Configured           = 5,
				HOST_STATE_Ready                = 6,
				HOST_STATE_Suspended            = 7,
			};
			
			enum USB_Host_ErrorCodes
			{
				HOST_ERROR_VBusVoltageDip       = 0,
			};
			
			enum USB_Host_WaitMSErrorCodes
			{
				HOST_WAITERROR_Sucessful        = 0,
				HOST_WAITERROR_DeviceDisconnect = 1,
				HOST_WAITERROR_PipeError        = 2,
				HOST_WAITERROR_SetupStalled     = 3,
			};

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define USB_HOST_HostModeOn()              MACROS{ USBCON |=  (1 << HOST);           }MACROE
			#define USB_HOST_HostModeOff()             MACROS{ USBCON &= ~(1 << HOST);           }MACROE

			#define USB_HOST_VBUS_On()                 MACROS{ UHWCON |=  (1 << UVCONE); OTGCON |= (1 << VBUSREQ); }MACROE
			#define USB_HOST_VBUS_Off()                MACROS{ OTGCON |=  (1 << VBUSRQC);        }MACROE 

			#define USB_HOST_ResetDevice()             MACROS{ UHCON  |=  (1 << RESET);          }MACROE
			#define USB_HOST_SetDeviceAddress(addr)    MACROS{ UHADDR  =  (addr & 0b00111111);   }MACROE

		/* Function Prototypes: */
			uint8_t USB_Host_WaitMS(uint8_t MS);
			void    USB_Host_ResetDevice(void);

#endif
