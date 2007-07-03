#ifndef USBLOWLEVEL_H
#define USBLOWLEVEL_H

	/* Includes */
		#include <avr/io.h>
		#include <avr/interrupt.h>

		#include "../USB.h"

	/* Public Macros */
		#define USB_MODE_DEVICE         (1 << UIMOD)
		#define USB_MODE_HOST           0

	/* Private Macros */
		#define USB_PLL_On()            PLLCSR  =  ((1 << PLLP1) | (1 << PLLP0) | (1 << PLLE))
		#define USB_PLL_Off()           PLLCSR  =  0
		#define USB_PLL_IsReady()       PLLCSR &   (1 << PLOCK)

		#define USB_REG_On()            UHWCON |=  (1 << UVREGE)
		#define USB_REG_Off()           UHWCON &= ~(1 << UVREGE)
	
		#define USB_CLK_Freeze()        USBCON |=  (1 << FRZCLK)
		#define USB_CLK_Unfreeze()      USBCON &= ~(1 << FRZCLK)

		#define USB_Interface_Enable()  USBCON |=  (1 << USBE)
		#define USB_Interface_Disable() USBCON &= ~(1 << USBE)

		#define USB_Get_VBUS_Status()   (USBSTA & (1 << VBUS))
		
		
		#define USB_DEV_Detach()       UDCON |=  (1 << DETACH)
		#define USB_DEV_Attach()       UDCON &= ~(1 << DETACH)
		
	/* Inline Functions */
		static inline void USB_LowLevelInit(const uint8_t Mode)
		{
			UHWCON = ((1 << UIDE) | Mode);
		}
	
		static inline void USB_PowerOn(void)
		{
			USB_REG_On();
			USB_PLL_On();
			
			while (!(USB_PLL_IsReady()));
			
			USB_CLK_Unfreeze();
			USB_Interface_Enable();
			
			USB_DEV_Attach();
		}

#endif
