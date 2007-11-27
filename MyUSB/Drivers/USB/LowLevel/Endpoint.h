/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../../Common/FunctionAttributes.h"
		#include "../../../Common/Common.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define ENDPOINT_CONFIG_OK               true
			#define ENDPOINT_CONFIG_FAIL             false

			#define ENDPOINT_TYPE_CONTROL            0b00
			#define ENDPOINT_TYPE_ISOCHRONOUS        0b01
			#define ENDPOINT_TYPE_BULK               0b10
			#define ENDPOINT_TYPE_INTERRUPT          0b11

			#define ENDPOINT_DIR_OUT                 0
			#define ENDPOINT_DIR_IN                  (1 << EPDIR)
			
			#define ENDPOINT_DESCRIPTOR_DIR_IN       0x80
			#define ENDPOINT_DESCRIPTOR_DIR_OUT      0x00		
			
			#define ENDPOINT_SIZE_8_MASK             0b000
			#define ENDPOINT_SIZE_16_MASK            0b001
			#define ENDPOINT_SIZE_32_MASK            0b010
			#define ENDPOINT_SIZE_64_MASK            0b011
			#define ENDPOINT_SIZE_128_MASK           0b100
			#define ENDPOINT_SIZE_256_MASK           0b101
			#define ENDPOINT_SIZE_512_MASK           0b110
			
			#define ENDPOINT_BANK_SINGLE             0
			#define ENDPOINT_BANK_DOUBLE             (1 << EPBK0)
			
			#define ENDPOINT_CONTROLEP               0
			#define ENDPOINT_CONTROLEP_SIZE          64
			
			#define ENDPOINT_EPNUM_MASK              0b111
			#define ENDPOINT_MAXENDPOINTS            7
			
			#define Endpoint_BytesInEndpoint()               UEBCX
			#define Endpoint_GetCurrentEndpoint()           (UENUM & ENDPOINT_EPNUM_MASK)
			#define Endpoint_SelectEndpoint(epnum)   MACROS{ UENUM   = (epnum & ENDPOINT_EPNUM_MASK); }MACROE
			#define Endpoint_ResetFIFO(epnum)        MACROS{ UERST   = (1 << epnum); UERST = 0;       }MACROE
			#define Endpoint_EnableEndpoint()        MACROS{ UECONX |=  (1 << EPEN);                  }MACROE
			#define Endpoint_DisableEndpoint()       MACROS{ UECONX &= ~(1 << EPEN);                  }MACROE
			#define Endpoint_IsEnabled()                     ((UECONX &   (1 << EPEN)) ? true : false)
			#define Endpoint_Reset(epnum)            MACROS{ UENUM = epnum; uint8_t temp = UECONX; UECONX = (temp | (1 << EPEN)); UECONX = (temp & ~(1 << EPEN)); }MACROE
			#define Endpoint_AllocateMemory()        MACROS{ UECFG1X |=  (1 << ALLOC);                }MACROE
			#define Endpoint_DeallocateMemory()      MACROS{ UECFG1X &= ~(1 << ALLOC);                }MACROE
			
			#define Endpoint_ReadWriteAllowed()            ((UEINTX & (1 << RWAL)) ? true : false)

			
			#define Endpoint_ConfigureEndpoint(num, type, dir, size, banks)                           \
												       Endpoint_ConfigureEndpoint_P(num,              \
                                                       ((type << EPTYPE0) | dir),                     \
											           ((Endpoint_BytesToEPSizeMask(size) << EPSIZE0) | banks))
			#define Endpoint_IsConfigured()                ((UESTA0X & (1 << CFGOK)) ? ENDPOINT_CONFIG_OK : ENDPOINT_CONFIG_FAIL)

			#define Endpoint_FIFOCON_Clear()         MACROS{ UEINTX  &= ~(1 << FIFOCON);              }MACROE
			
			#define Endpoint_In_Clear()              MACROS{ uint8_t temp = UEINTX; UEINTX = (temp & ~(1 << TXINI)); UEINTX = (temp & ~(1 << FIFOCON));  }MACROE
			#define Endpoint_Out_Clear()             MACROS{ uint8_t temp = UEINTX; UEINTX = (temp & ~(1 << RXOUTI)); UEINTX = (temp & ~(1 << FIFOCON)); }MACROE

			#define Endpoint_In_IsReady()                  ((UEINTX  &   (1 << TXINI)) ? true : false)
			#define Endpoint_Out_IsRecieved()              ((UEINTX  &   (1 << RXOUTI)) ? true: false)
			
			#define Endpoint_ClearSetupRecieved()    MACROS{ UEINTX  &= ~(1 << RXSTPI);               }MACROE
			#define Endpoint_Stall_Transaction()     MACROS{ UECONX  |=  (1 << STALLRQ);              }MACROE
			
		/* Function Prototypes: */
			void Endpoint_ClearEndpoints(void);

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define Endpoint_IsSetupRecieved()             ((UEINTX  &   (1 << RXSTPI)) ? true : false)
	
		/* Inline Functions: */
			static inline uint8_t Endpoint_BytesToEPSizeMask(const uint16_t Bytes)
			                                                 ATTR_WARN_UNUSED_RESULT ATTR_CONST;
			static inline uint8_t Endpoint_BytesToEPSizeMask(const uint16_t Bytes)
			{
				if (Bytes <= 8)
				  return ENDPOINT_SIZE_8_MASK;
				else if (Bytes <= 16)
				  return ENDPOINT_SIZE_16_MASK;
				else if (Bytes <= 32)
				  return ENDPOINT_SIZE_32_MASK;
				else if (Bytes <= 64)
				  return ENDPOINT_SIZE_64_MASK;
				else if (Bytes <= 128)
				  return ENDPOINT_SIZE_128_MASK;
				else if (Bytes <= 256)
				  return ENDPOINT_SIZE_256_MASK;
				else
				  return ENDPOINT_SIZE_512_MASK;
			};

		/* Function Prototypes: */
			bool Endpoint_ConfigureEndpoint_P(const uint8_t EndpointNum, const uint8_t UECFG0Xdata, const uint8_t UECFG1Xdata);

#endif
