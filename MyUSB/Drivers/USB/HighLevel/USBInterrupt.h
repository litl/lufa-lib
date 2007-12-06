/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef USBINT_H
#define USBINT_H

	/* Includes: */
		#include <avr/io.h>
		
		#include "../LowLevel/LowLevel.h"
		#include "../HighLevel/Events.h"
		#include "../../../Common/Common.h"
		
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define ENDPOINT_PIPE_vect                       USB_COM_vect
	
			#define USB_INT_Enable(int)              MACROS{ USB_INT_GET_EN_REG(int)   |=   USB_INT_GET_EN_MASK(int);   }MACROE
			#define USB_INT_Disable(int)             MACROS{ USB_INT_GET_EN_REG(int)   &= ~(USB_INT_GET_EN_MASK(int));  }MACROE
			#define USB_INT_Clear(int)               MACROS{ USB_INT_GET_INT_REG(int)  &= ~(USB_INT_GET_INT_MASK(int)); }MACROE
			#define USB_INT_IsEnabled(int)                 ((USB_INT_GET_EN_REG(int)   &    USB_INT_GET_EN_MASK(int)) ? true : false)
			#define USB_INT_HasOccurred(int)               ((USB_INT_GET_INT_REG(int)  &    USB_INT_GET_INT_MASK(int)) ? true : false)
		
		/* Throwable Events: */
			RAISES_EVENT(USB_VBUSChange);
			RAISES_EVENT(USB_VBUSConnect);
			RAISES_EVENT(USB_VBUSDisconnect);

			#if !defined(USB_HOST_ONLY) // All modes or USB_DEVICE_ONLY
				RAISES_EVENT(USB_Suspend);
				RAISES_EVENT(USB_WakeUp);
				RAISES_EVENT(USB_Reset);
			#endif
			
			#if !defined(USB_DEVICE_ONLY) // All modes or USB_HOST_ONLY
				RAISES_EVENT(USB_HostError);
				RAISES_EVENT(USB_DeviceUnattached);
			#endif

			#if !defined(USB_DEVICE_ONLY) && !defined(USB_HOST_ONLY) // All modes
				RAISES_EVENT(USB_UIDChange);
			#endif
			
	/* Private Interface - For use in library only: */
		/* Inline Functions: */
			static inline void USB_INT_DisableAllInterrupts(void)
			{
				USBCON &= ~((1 << VBUSTE) | (1 << IDTE));
				UHIEN   = 0;
				UDIEN   = 0;
				OTGIEN  = 0;
			}

			static inline void USB_INT_ClearAllInterrupts(void)
			{
				USBINT  = 0;
				UHINT   = 0;
				UDINT   = 0;
				OTGINT  = 0;
			}

		/* Macros: */
			#define USB_INT_GET_EN_REG(a, b, c, d)           a
			#define USB_INT_GET_EN_MASK(a, b, c, d)          b
			#define USB_INT_GET_INT_REG(a, b, c, d)          c
			#define USB_INT_GET_INT_MASK(a, b, c, d)         d

			#define USB_INT_VBUS                             USBCON, (1 << VBUSTE) , USBINT, (1 << VBUSTI)
			#define USB_INT_IDTI                             USBCON, (1 << IDTE)   , USBINT, (1 << IDTI)
			#define USB_INT_WAKEUP                           UDIEN , (1 << WAKEUPE), UDINT , (1 << WAKEUPI)
			#define USB_INT_SUSPEND                          UDIEN , (1 << SUSPE)  , UDINT , (1 << SUSPI)
			#define USB_INT_EORSTI                           UDIEN , (1 << EORSTE) , UDINT , (1 << EORSTI)
			#define USB_INT_SRPI                             OTGIEN, (1 << SRPE)   , OTGINT, (1 << SRPI)
			#define USB_INT_DCONNI                           UHIEN , (1 << DCONNE) , UHINT , (1 << DCONNI)
			#define USB_INT_DDISCI                           UHIEN , (1 << DDISCE) , UHINT , (1 << DDISCI)
			#define USB_INT_BCERRI                           OTGIEN, (1 << BCERRE) , OTGINT, (1 << BCERRI)
			#define USB_INT_VBERRI                           OTGIEN, (1 << VBERRE) , OTGINT, (1 << VBERRI)
			#define USB_INT_SOFI                             UDIEN,  (1 << SOFE)   , UDINT , (1 << SOFI)
			#define USB_INT_HSOFI                            UHIEN,  (1 << HSOFE)  , UHINT , (1 << HSOFI)
			#define USB_INT_RSTI                             UHIEN , (1 << RSTE)   , UHINT , (1 << RSTI)

#endif
