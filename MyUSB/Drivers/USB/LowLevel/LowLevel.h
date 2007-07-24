#ifndef USBLOWLEVEL_H
#define USBLOWLEVEL_H

	/* Includes */
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>

		#include "Device.h"
		#include "Endpoint.h"
		#include "Chapter9.h"
		#include "../HighLevel/USBInterrupt.h"
		#include "../HighLevel/USBTask.h"

	/* Preprocessor Checks and Defines */
		#if (F_CPU == 8000000)
			#define USB_PLL_PSC            ((1 << PLLP1) | (1 << PLLP0))
		#elif (F_CPU == 16000000)
			#define USB_PLL_PSC            ((1 << PLLP2) | (1 << PLLP1))
		#else
			#error No PLL prescale value avaliable for chosen F_CPU value.
		#endif

	/* Public Macros */
		#define USB_MODE_NONE              0
		#define USB_MODE_DEVICE            1
		#define USB_MODE_HOST              2
		#define USB_MODE_UID               3
		
		#define USB_POWERON_OK             true
		#define USB_POWERON_FAIL           false
						
		#define USB_VBUS_GetStatus()       ((USBSTA & (1 << VBUS)) ? true : false)

	/* Private Macros */	
		#define USB_PLL_On()               PLLCSR  =  (USB_PLL_PSC | (1 << PLLE))
		#define USB_PLL_Off()              PLLCSR  =  0
		#define USB_PLL_IsReady()          PLLCSR &   (1 << PLOCK)

		#define USB_REG_On()               UHWCON |=  (1 << UVREGE)
		#define USB_REG_Off()              UHWCON &= ~(1 << UVREGE)
	
		#define USB_OTGPAD_On()            USBCON |=  (1 << OTGPADE)
		#define USB_OTGPAD_Off()           USBCON &= ~(1 << OTGPADE)

		#define USB_CLK_Freeze()           USBCON |=  (1 << FRZCLK)
		#define USB_CLK_Unfreeze()         USBCON &= ~(1 << FRZCLK)

		#define USB_Interface_Enable()     USBCON |=  (1 << USBE)
		#define USB_Interface_Disable()    USBCON &= ~(1 << USBE)
		#define USB_Interface_IsEnabled()  USBCON &   (1 << USBE)
		
		#define USB_FIFOCON_Clear()        UEINTX &= ~(1<<FIFOCON)
		
		#define USB_In_Clear()             UEINTX &= ~(1 << TXINI)
		#define USB_In_IsReady()           UEINTX &   (1 << TXINI)
		
		#define USB_Out_Clear()            UEINTX &= ~(1<<RXOUTI), USB_FIFOCON_Clear()
		#define USB_Out_IsRecieved()       UEINTX &   (1<<RXOUTI)
		
		#define USB_Stall_Transaction()    UECONX |=  (1<<STALLRQ)

	/* Inline Functions */		
		static inline uint8_t USB_Read_Byte(void)
		{
			return UEDATX;
		}

		static inline void USB_Write_Byte(uint8_t Byte)
		{
			UEDATX = Byte;
		}

		static inline void USB_Ignore_Byte(void)
		{		
			volatile uint8_t Dummy;
			
			Dummy = UEDATX;
			
			// TODO: Optimize with inline asm - PUSH LDS POP
		}
		
		static inline uint16_t USB_Read_Word(void)
		{
			uint16_t Data;
			
			Data  = UEDATX;
			Data |= (((uint16_t)UEDATX) << 8);
		
			return Data;
		}

		static inline void USB_Write_Word(uint16_t Byte)
		{
			UEDATX = (Byte & 0xFF);
			UEDATX = (Byte >> 8);
		}
		
		static inline void USB_Ignore_Word(void)
		{
			volatile uint8_t Dummy;
			
			Dummy = UEDATX;
			Dummy = UEDATX;

			// TODO: Optimize with inline asm - PUSH LDS LDS POP
		}

	/* Function Prototypes */
		void    USB_Init(const uint8_t Mode, const uint8_t Options);
		bool    USB_PowerOn(void);
		void    USB_PowerOff(void);
		uint8_t USB_GetUSBMode(void);

#endif
