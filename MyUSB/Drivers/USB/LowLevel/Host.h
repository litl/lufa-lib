/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __HOST_H__
#define __HOST_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../../Common/Common.h"
		#include "../../../Common/FunctionAttributes.h"
		#include "../HighLevel/USBInterrupt.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define USB_HOST_DEVICEADDRESS        1
		
			#define USB_HOST_OPT_AUTOVBUS         (0 << 1)
			#define USB_HOST_OPT_MANUALVBUS       (1 << 1)

			#define USB_HOST_VBUS_On()            MACROS{ PORTE |=  (1 << 7);             }MACROE
			#define USB_HOST_VBUS_Off()           MACROS{ PORTE &= ~(1 << 7);             }MACROE
			
			#define USB_HOST_ResetBus()           MACROS{ UHCON |=  (1 << RESET);         }MACROE
			#define USB_HOST_ResetBus_IsDone()          ((UHCON &   (1 << RESET)) ? false : true)
			
		/* Enums: */
			enum USB_Host_States
			{
				HOST_STATE_Unattached           = 0,
				HOST_STATE_Attached             = 1,
				HOST_STATE_Powered              = 2,
				HOST_STATE_Default              = 3,
				HOST_STATE_Addressed            = 4,
				HOST_STATE_Configured           = 5,
				HOST_STATE_Suspended            = 6,
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
		
		/* Inline Functions: */
			static inline uint8_t USB_Host_Read_Byte(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t USB_Host_Read_Byte(void)
			{
				return UPDATX;
			}

			static inline void USB_Host_Write_Byte(const uint8_t Byte)
			{
				UPDATX = Byte;
			}

			static inline void USB_Host_Ignore_Byte(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UPDATX;
			}
			
			static inline uint16_t USB_Host_Read_Word(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint16_t USB_Host_Read_Word(void)
			{
				uint16_t Data;
				
				Data  = UPDATX;
				Data |= (((uint16_t)UPDATX) << 8);
			
				return Data;
			}

			static inline void USB_Host_Write_Word(const uint16_t Byte)
			{
				UPDATX = (Byte & 0xFF);
				UPDATX = (Byte >> 8);
			}
			
			static inline void USB_Host_Ignore_Word(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UPDATX;
				Dummy = UPDATX;
			}

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define USB_HOST_HostModeOn()              MACROS{ USBCON |=  (1 << HOST);           }MACROE
			#define USB_HOST_HostModeOff()             MACROS{ USBCON &= ~(1 << HOST);           }MACROE

			#define USB_HOST_ManualVBUS_Enable()       MACROS{ UHWCON &= ~(1 << UVCONE); OTGCON |= (1 << VBUSHWC); DDRE |= (1 << 7); }MACROE
			#define USB_HOST_ManualVBUS_Disable()      MACROS{ OTGCON &= ~(1 << VBUSHWC);        }MACROE

			#define USB_HOST_AutoVBUS_On()             MACROS{ UHWCON |=  (1 << UVCONE); OTGCON |= (1 << VBUSREQ); }MACROE
			#define USB_HOST_AutoVBUS_Off()            MACROS{ OTGCON |=  (1 << VBUSRQC);        }MACROE 

			#define USB_HOST_SOFGeneration_Enable()    MACROS{ UHCON  |=  (1 << SOFEN);          }MACROE 
			#define USB_HOST_SOFGeneration_Disable()   MACROS{ UHCON  &= ~(1 << SOFEN);          }MACROE 
			#define USB_HOST_SOFGeneration_IsEnabled()       ((UHCON  &   (1 << SOFEN)) ? true : false)

			#define USB_HOST_ResetDevice()             MACROS{ UHCON  |=  (1 << RESET);          }MACROE
			#define USB_HOST_SetDeviceAddress(addr)    MACROS{ UHADDR  =  (addr & 0b00111111);   }MACROE

		/* Function Prototypes: */
			uint8_t USB_Host_WaitMS(uint8_t MS);
			void    USB_Host_ResetDevice(void);

#endif
