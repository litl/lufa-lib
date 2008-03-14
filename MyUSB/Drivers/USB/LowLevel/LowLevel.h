/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __USBLOWLEVEL_H__
#define __USBLOWLEVEL_H__

	/* Includes: */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>
		
		#include "../../../Common/Common.h"
		#include "../HighLevel/Events.h"
		#include "../HighLevel/USBTask.h"
		#include "../HighLevel/USBInterrupt.h"
		#include "USBMode.h"
		
		#if defined(USB_CAN_BE_HOST)
			#include "Host.h"
			#include "Pipe.h"
			#include "OTG.h"
		#endif
		
		#if defined(USB_CAN_BE_DEVICE)
			#include "Device.h"
			#include "Endpoint.h"
			#include "DevChapter9.h"
		#endif

	/* Preprocessor Checks and Defines: */
		#if (F_CPU == 8000000)
			#if (defined(__AVR_AT90USB82__) || defined(__AVR_AT90USB162__))
				#define USB_PLL_PSC                0
			#else
				#define USB_PLL_PSC                ((1 << PLLP1) | (1 << PLLP0))
			#endif
		#elif (F_CPU == 16000000)
			#if (defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB647__))
				#define USB_PLL_PSC                ((1 << PLLP2) | (1 << PLLP1))
			#elseif (defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1287__))
				#define USB_PLL_PSC                ((1 << PLLP2) | (1 << PLLP0))
			#else if (defined(__AVR_AT90USB82__) || defined(__AVR_AT90USB162__))
				#define USB_PLL_PSC                (1 << PLLP0)
			#endif
		#endif
		
		#if !defined(USB_PLL_PSC)
			#error No PLL prescale value avaliable for chosen F_CPU value and AVR model.
		#endif
		
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define USB_MODE_NONE                      0
			#define USB_MODE_DEVICE                    1
			#define USB_MODE_HOST                      2
			#define USB_MODE_UID                       3
			
			#define USB_OPT_REG_DISABLED               (1 << 1)
			#define USB_OPT_REG_ENABLED                (0 << 1)

			#define EP_TYPE_CONTROL                    0b00
			#define EP_TYPE_ISOCHRONOUS                0b01
			#define EP_TYPE_BULK                       0b10
			#define EP_TYPE_INTERRUPT                  0b11
			#define EP_TYPE_MASK                       0b11

			#define USB_VBUS_GetStatus()             ((USBSTA & (1 << VBUS)) ? true : false)
	
			#define USB_Detach()               MACROS{ UDCON  |=  (1 << DETACH);  }MACROE
			#define USB_Attach()               MACROS{ UDCON  &= ~(1 << DETACH);  }MACROE

		/* Function Prototypes: */
			void USB_Init(
			               #if defined(USB_CAN_BE_BOTH)
			               const uint8_t Mode
						   #endif

			               #if (defined(USB_CAN_BE_BOTH) && !defined(USE_STATIC_OPTIONS))
			               ,
						   #elif (!defined(USB_CAN_BE_BOTH) && defined(USE_STATIC_OPTIONS))
						   void
			               #endif
						   
			               #if !defined(USE_STATIC_OPTIONS)
			               const uint8_t Options
			               #endif
			               );
			
			void USB_ShutDown(void);

		/* Enums: */
			enum USB_PowerOnErrorCodes_t
			{
				POWERON_ERROR_NoUSBModeSpecified       = 0,
			};

		/* Global Variables: */
			#if (!defined(USB_HOST_ONLY) && !defined(USB_DEVICE_ONLY))
				extern volatile uint8_t USB_CurrentMode;
			#endif
			
			#if !defined(USE_STATIC_OPTIONS)
				extern volatile uint8_t USB_Options;
			#endif

		/* Throwable Events: */
			RAISES_EVENT(USB_Disconnect);
			
			#if defined(USB_CAN_BE_BOTH)
				RAISES_EVENT(USB_PowerOnFail);
			#endif
			
	/* Private Interface - For use in library only: */
		/* Macros: */
			#define USB_PLL_On()               MACROS{ PLLCSR   =  (USB_PLL_PSC | (1 << PLLE)); }MACROE
			#define USB_PLL_Off()              MACROS{ PLLCSR   =  0;                           }MACROE
			#define USB_PLL_IsReady()                ((PLLCSR  &   (1 << PLOCK)) ? true : false)

			#if defined(USB_FULL_CONTROLLER)		
				#define USB_REG_On()           MACROS{ UHWCON  |=  (1 << UVREGE);               }MACROE
				#define USB_REG_Off()          MACROS{ UHWCON  &= ~(1 << UVREGE);               }MACROE
			#else
				#define USB_REG_On()           MACROS{ REGCR   &= ~(1 << REGDIS);               }MACROE
				#define USB_REG_Off()          MACROS{ REGCR   |=  (1 << REGDIS);               }MACROE			
			#endif
			
			#define USB_OTGPAD_On()            MACROS{ USBCON  |=  (1 << OTGPADE);              }MACROE
			#define USB_OTGPAD_Off()           MACROS{ USBCON  &= ~(1 << OTGPADE);              }MACROE

			#define USB_CLK_Freeze()           MACROS{ USBCON  |=  (1 << FRZCLK);               }MACROE
			#define USB_CLK_Unfreeze()         MACROS{ USBCON  &= ~(1 << FRZCLK);               }MACROE

			#define USB_Interface_Enable()     MACROS{ USBCON  |=  (1 << USBE);                 }MACROE
			#define USB_Interface_Disable()    MACROS{ USBCON  &= ~(1 << USBE);                 }MACROE
			#define USB_Interface_Reset()      MACROS{ uint8_t Temp = USBCON; USBCON = (Temp & ~(1 << USBE)); \
			                                                     USBCON = (Temp | (1 << USBE)); }MACROE
	
		/* Inline Functions: */		
			static inline uint8_t USB_GetUSBModeFromUID(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t USB_GetUSBModeFromUID(void)
			{
				#if defined(USB_FULL_CONTROLLER)
				if (USBSTA & (1 << ID))
				  return USB_MODE_DEVICE;
				else
				  return USB_MODE_HOST;
				#else
				return USB_MODE_DEVICE;
				#endif
			}

		/* Function Prototypes: */
			void USB_SetupInterface(void);
			
			#ifdef INCLUDE_FROM_LOWLEVEL_C
				static void USB_ResetInterface(void);
			#endif
			
#endif
