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
		#include <util/delay.h>

		#include "../../../Common/Common.h"
		#include "../HighLevel/USBInterrupt.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define USB_HOST_DEVICEADDRESS             1
			#define USB_HOST_TIMEOUT_MS                1000
			
			#define USB_Host_ResetBus()                MACROS{ UHCON |=  (1 << RESET);          }MACROE
			#define USB_Host_ResetBus_IsDone()               ((UHCON &   (1 << RESET)) ? false : true)
			
			#define USB_Host_SOFGeneration_Enable()    MACROS{ UHCON |=  (1 << SOFEN);          }MACROE 
			#define USB_Host_SOFGeneration_Disable()   MACROS{ UHCON &= ~(1 << SOFEN);          }MACROE 
			#define USB_Host_SOFGeneration_IsEnabled()       ((UHCON &   (1 << SOFEN)) ? true : false)
		
			#define USB_Host_IsDeviceFullSpeed()             ((USBSTA &  (1 << SPEED)) ? true : false)

			#define USB_Host_IsRemoteWakeupSent()            ((UHINT &   (1 << RXRSMI)) ? true : false)
			#define USB_Host_ClearRemoteWakeupSent()   MACROS{ UHINT &= ~(1 << RXRSMI);         }MACROE
			#define USB_Host_SendResume()              MACROS{ UHCON |=  (1 << RESUME);         }MACROE
			
		/* Enums: */
			enum USB_Host_States_t
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
			
			enum USB_Host_ErrorCodes_t
			{
				HOST_ERROR_VBusVoltageDip       = 0,
			};
			
			enum USB_Host_EnumerationErrorCodes_t
			{
				HOST_ENUMERROR_NoError          = 0,
				HOST_ENUMERROR_WaitStage        = 1,
				HOST_ENUMERROR_NoDeviceDetected = 2,
				HOST_ENUMERROR_ControlError     = 3,
				HOST_ENUMERROR_PipeConfigError  = 4,
			};

			enum USB_Host_WaitMSErrorCodes_t
			{
				HOST_WAITERROR_Successful       = 0,
				HOST_WAITERROR_DeviceDisconnect = 1,
				HOST_WAITERROR_PipeError        = 2,
				HOST_WAITERROR_SetupStalled     = 3,
			};

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define USB_Host_HostMode_On()          MACROS{ USBCON |=  (1 << HOST);           }MACROE
			#define USB_Host_HostMode_Off()         MACROS{ USBCON &= ~(1 << HOST);           }MACROE

			#define USB_Host_VBUS_Auto_Enable()     MACROS{ OTGCON &= ~(1 << VBUSHWC); UHWCON |=  (1 << UVCONE);                    }MACROE
			#define USB_Host_VBUS_Manual_Enable()   MACROS{ OTGCON |=  (1 << VBUSHWC); UHWCON &= ~(1 << UVCONE); DDRE |= (1 << 7); }MACROE

			#define USB_Host_VBUS_Auto_On()         MACROS{ OTGCON |= (1 << VBUSREQ);         }MACROE
			#define USB_Host_VBUS_Manual_On()       MACROS{ PORTE  |= (1 << 7);               }MACROE

			#define USB_Host_VBUS_Auto_Off()        MACROS{ OTGCON |= (1 << VBUSRQC);         }MACROE
			#define USB_Host_VBUS_Manual_Off()      MACROS{ PORTE  &= ~(1 << 7);              }MACROE

			#define USB_Host_SetDeviceAddress(addr) MACROS{ UHADDR  =  (addr & 0b00111111);   }MACROE

		/* Function Prototypes: */
			uint8_t USB_Host_WaitMS(uint8_t MS);
			void    USB_Host_ResetDevice(void);
			void    USB_Host_PrepareForDeviceConnect(void);
	#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif
