/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef USBLOWLEVEL_H
#define USBLOWLEVEL_H

	/* Includes */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>

		#include "Device.h"
		#include "Host.h"
		#include "Endpoint.h"
		#include "Chapter9.h"
		#include "../HighLevel/USBInterrupt.h"
		#include "../HighLevel/USBTask.h"
		#include "../../../Common/FunctionAttributes.h"
		#include "../../../Common/Common.h"
		
	/* Preprocessor Checks and Defines */
		#if (F_CPU == 8000000)
			#define USB_PLL_PSC                    ((1 << PLLP1) | (1 << PLLP0))
		#elif (F_CPU == 16000000)
			#define USB_PLL_PSC                    ((1 << PLLP2) | (1 << PLLP1))
		#else
			#error No PLL prescale value avaliable for chosen F_CPU value.
		#endif

	/* Public Macros */
		#define USB_MODE_NONE                      0
		#define USB_MODE_DEVICE                    1
		#define USB_MODE_HOST                      2
		#define USB_MODE_UID                       3
				
		#define USB_SETUPINTERFACE_OK              true
		#define USB_SETUPINTERFACE_FAIL            false
						
		#define USB_VBUS_GetStatus()               ((USBSTA & (1 << VBUS)) ? true : false)

		#define USB_FIFOCON_Clear()        MACROS{ UEINTX  &= ~(1<<FIFOCON);   }MACROE
		
		#define USB_In_Clear()             MACROS{ UEINTX  &= ~(1 << TXINI); USB_FIFOCON_Clear();  }MACROE
		#define USB_In_IsReady()                   (UEINTX &   (1 << TXINI))
		
		#define USB_Out_Clear()            MACROS{ UEINTX  &= ~(1 << RXOUTI); USB_FIFOCON_Clear(); }MACROE
		#define USB_Out_IsRecieved()               (UEINTX &   (1 << RXOUTI))
		
		#define USB_ClearSetupRecieved()   MACROS{ UEINTX  &= ~(1 << RXSTPI);  }MACROE
		#define USB_Stall_Transaction()    MACROS{ UECONX  |=  (1 << STALLRQ); }MACROE
		#define USB_IsSetupRecieved()              (UEINTX & (1 << RXSTPI))
		
		#define USB_Detach()               MACROS{ UDCON   |=  (1 << DETACH);  }MACROE
		#define USB_Attach()               MACROS{ UDCON   &= ~(1 << DETACH);  }MACROE
		
	/* Private Macros */
		#define USB_PLL_On()               MACROS{ PLLCSR   =  (USB_PLL_PSC | (1 << PLLE)); }MACROE
		#define USB_PLL_Off()              MACROS{ PLLCSR   =  0;              }MACROE
		#define USB_PLL_IsReady()                  (PLLCSR &  (1 << PLOCK))

		#define USB_REG_On()               MACROS{ UHWCON  |=  (1 << UVREGE);  }MACROE
		#define USB_REG_Off()              MACROS{ UHWCON  &= ~(1 << UVREGE);  }MACROE
	
		#define USB_OTGPAD_On()            MACROS{ USBCON  |=  (1 << OTGPADE); }MACROE
		#define USB_OTGPAD_Off()           MACROS{ USBCON  &= ~(1 << OTGPADE); }MACROE

		#define USB_CLK_Freeze()           MACROS{ USBCON  |=  (1 << FRZCLK);  }MACROE
		#define USB_CLK_Unfreeze()         MACROS{ USBCON  &= ~(1 << FRZCLK);  }MACROE

		#define USB_Interface_Enable()     MACROS{ USBCON  |=  (1 << USBE);    }MACROE
		#define USB_Interface_Disable()    MACROS{ USBCON  &= ~(1 << USBE);    }MACROE
		#define USB_Interface_IsEnabled()          (USBCON &   (1 << USBE))
	
	/* Enums */
		enum PowerOnErrorCodes
		{
			POWERON_ERR_NoUSBModeSpecified      = 0,
			POWERON_ERR_EndpointCreationFailed  = 1,
		};
	
	/* External Variables */
		extern volatile uint8_t USB_CurrentMode;
		extern          uint8_t USB_Options;
	
	/* Inline Functions */		
		static inline uint8_t USB_GetUSBModeFromUID(void) ATTR_WARN_UNUSED_RESULT;
		static inline uint8_t USB_GetUSBModeFromUID(void)
		{
			if (USBSTA & (1 << ID))
			  return USB_MODE_DEVICE;
			else
			  return USB_MODE_HOST;
		}

	/* Function Prototypes */
		void USB_Init(const uint8_t Mode, const uint8_t Options);
		void USB_ShutDown(void);
		bool USB_SetupInterface(void);
		bool USB_HostWaitMS(uint8_t MS);

#endif
