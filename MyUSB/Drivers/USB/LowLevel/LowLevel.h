/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __USBLOWLEVEL_H__
#define __USBLOWLEVEL_H__

	/* Includes: */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>

		#include "../HighLevel/Events.h"
		#include "../HighLevel/USBTask.h"
		#include "../HighLevel/USBInterrupt.h"
		
		#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
			#include "Host.h"
			#include "Pipe.h"
		#endif
		
		#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
			#include "Device.h"
			#include "Endpoint.h"
			#include "DevChapter9.h"
		#endif		
		
		#include "../../../Common/FunctionAttributes.h"
		#include "../../../Common/Common.h"
		
	/* Preprocessor Checks and Defines: */
		#if (F_CPU == 8000000)
			#define USB_PLL_PSC                    ((1 << PLLP1) | (1 << PLLP0))
		#elif (F_CPU == 16000000)
			#define USB_PLL_PSC                    ((1 << PLLP2) | (1 << PLLP1))
		#else
			#error No PLL prescale value avaliable for chosen F_CPU value.
		#endif
		
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define USB_MODE_NONE                      0
			#define USB_MODE_DEVICE                    1
			#define USB_MODE_HOST                      2
			#define USB_MODE_UID                       3
			
			#define USB_OPT_REG_DISABLED               (1 << 1)
			#define USB_OPT_REG_ENABLED                (0 << 1)
					
			#define USB_SETUPINTERFACE_OK              true
			#define USB_SETUPINTERFACE_FAIL            false

			#define EP_TYPE_CONTROL                    0b00
			#define EP_TYPE_ISOCHRONOUS                0b01
			#define EP_TYPE_BULK                       0b10
			#define EP_TYPE_INTERRUPT                  0b11

			#define USB_VBUS_GetStatus()             ((USBSTA  & (1 << VBUS)) ? true : false)
	
			#define USB_Detach()               MACROS{ UDCON   |=  (1 << DETACH);  }MACROE
			#define USB_Attach()               MACROS{ UDCON   &= ~(1 << DETACH);  }MACROE

		/* Function Prototypes: */
			void USB_Init(const uint8_t Mode, const uint8_t Options);
			void USB_ShutDown(void);

		/* Enums: */
			enum USB_PowerOnErrorCodes
			{
				POWERON_ERROR_NoUSBModeSpecified          = 0,
				POWERON_ERROR_UnavailableUSBModeSpecified = 1,
			};

		/* Global Variables: */
			extern volatile uint8_t USB_CurrentMode;
			extern          uint8_t USB_Options;

		/* Throwable Events: */
			RAISES_EVENT(USB_Disconnect);
			RAISES_EVENT(USB_PowerOnFail);

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define USB_PLL_On()               MACROS{ PLLCSR   =  (USB_PLL_PSC | (1 << PLLE)); }MACROE
			#define USB_PLL_Off()              MACROS{ PLLCSR   =  0;              }MACROE
			#define USB_PLL_IsReady()                ((PLLCSR  &   (1 << PLOCK)) ? true : false)

			#define USB_REG_On()               MACROS{ UHWCON  |=  (1 << UVREGE);  }MACROE
			#define USB_REG_Off()              MACROS{ UHWCON  &= ~(1 << UVREGE);  }MACROE
		
			#define USB_OTGPAD_On()            MACROS{ USBCON  |=  (1 << OTGPADE); }MACROE
			#define USB_OTGPAD_Off()           MACROS{ USBCON  &= ~(1 << OTGPADE); }MACROE

			#define USB_CLK_Freeze()           MACROS{ USBCON  |=  (1 << FRZCLK);  }MACROE
			#define USB_CLK_Unfreeze()         MACROS{ USBCON  &= ~(1 << FRZCLK);  }MACROE

			#define USB_Interface_Enable()     MACROS{ USBCON  |=  (1 << USBE);    }MACROE
			#define USB_Interface_Disable()    MACROS{ USBCON  &= ~(1 << USBE);    }MACROE
	
		/* Inline Functions: */		
			static inline uint8_t USB_GetUSBModeFromUID(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t USB_GetUSBModeFromUID(void)
			{
				if (USBSTA & (1 << ID))
				  return USB_MODE_DEVICE;
				else
				  return USB_MODE_HOST;
			}

		/* Function Prototypes: */
			bool USB_SetupInterface(void);
			void USB_ResetInterface(void);
			
#endif
