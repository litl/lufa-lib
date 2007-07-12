#ifndef USBLOWLEVEL_H
#define USBLOWLEVEL_H

	/* Includes */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>

		#include "Device.h"
		#include "Endpoint.h"
		#include "../HighLevel/USBTask.h"

	/* Preprocessor Checks and Defines */
		#if (F_CPU == 8000000)
			#define USB_PLL_PSC         ((1 << PLLP1) | (1 << PLLP0))
		#elif (F_CPU == 16000000)
			#define USB_PLL_PSC         ((1 << PLLP2) | (1 << PLLP1))
		#else
			#error No PLL prescale value avaliable for chosen F_CPU value.
		#endif

	/* Public Macros */
		#define USB_MODE_NONE             0
		#define USB_MODE_DEVICE           1
		#define USB_MODE_HOST             2
		#define USB_MODE_MIXED            3
		
		#define USB_POWERON_OK            true
		#define USB_POWERON_FAIL          false
						
	/* Private Macros */	
		#define USB_PLL_On()              PLLCSR  =  (USB_PLL_PSC | (1 << PLLE))
		#define USB_PLL_Off()             PLLCSR  =  0
		#define USB_PLL_IsReady()         PLLCSR &   (1 << PLOCK)

		#define USB_REG_On()              UHWCON |=  (1 << UVREGE)
		#define USB_REG_Off()             UHWCON &= ~(1 << UVREGE)
	
		#define USB_OTGPAD_On()           USBCON |=  (1 << OTGPADE)
		#define USB_OTGPAD_Off()          USBCON &= ~(1 << OTGPADE)

		#define USB_CLK_Freeze()          USBCON |=  (1 << FRZCLK)
		#define USB_CLK_Unfreeze()        USBCON &= ~(1 << FRZCLK)

		#define USB_Interface_Enable()    USBCON |=  (1 << USBE)
		#define USB_Interface_Disable()   USBCON &= ~(1 << USBE)

		#define USB_VBUS_GetStatus()      ((USBSTA & (1 << VBUS)) ? true : false)

		#define USB_INT_VBUS_Enable()     USBCON |= (1 << VBUSTE)
		#define USB_INT_VBUS_Disable()    USBCON &= ~(1 << VBUSTE)
		#define USB_INT_VBUS_IsEnabled()  (USBCON & (1 << VBUSTE))   
		#define USB_INT_VBUS_Reset()      USBINT = ~(1 << VBUSTI)
		
	/* Function Prototypes */
		void USB_Init(const uint8_t Mode, const uint8_t Options);
		void USB_Disable(void);
		uint8_t USB_GetUSBMode(void);

	/* Inline Functions */
	
		static inline bool USB_PowerOn(void)
		{
			if (USB_GetUSBMode() == USB_MODE_NONE)
			  return USB_POWERON_FAIL;
		
			USB_REG_On();
			USB_PLL_On();
			
			while (!(USB_PLL_IsReady()));
			
			USB_Interface_Disable();
			USB_Interface_Enable();
			USB_CLK_Unfreeze();

			if (USB_GetUSBMode() == USB_MODE_DEVICE)
			{
				if (Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, ENDPOINT_TYPE_CONTROL,
				                               ENDPOINT_DIR_OUT, ENDPOINT_SIZE_64, ENDPOINT_BANK_SINGLE)
				    == ENDPOINT_CONFIG_OK)
				{
					USB_DEV_Attach();
				}
				else
				{
					return USB_POWERON_FAIL;
				}
			}
			
			return USB_POWERON_OK;
		}

#endif
