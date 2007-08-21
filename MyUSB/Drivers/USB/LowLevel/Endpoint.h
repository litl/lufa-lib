/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef ENDPOINT_H
#define ENDPOINT_H

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../../Common/Common.h"

	/* Public Macros: */
		#define ENDPOINT_CONFIG_OK                 true
		#define ENDPOINT_CONFIG_FAIL               false

		#define ENDPOINT_TYPE_CONTROL              0b00
		#define ENDPOINT_TYPE_ISOCHRONOUS          0b01
		#define ENDPOINT_TYPE_BULK                 0b10
		#define ENDPOINT_TYPE_INTERRUPT            0b11

		#define ENDPOINT_DIR_OUT                   0
		#define ENDPOINT_DIR_IN                    (1 << EPDIR)
		
		#define ENDPOINT_DESCRIPTOR_DIR_IN         0x80
		#define ENDPOINT_DESCRIPTOR_DIR_OUT        0x00		
		
		#define ENDPOINT_SIZE_8_MASK               0b000
		#define ENDPOINT_SIZE_16_MASK              0b001
		#define ENDPOINT_SIZE_32_MASK              0b010
		#define ENDPOINT_SIZE_64_MASK              0b011
		#define ENDPOINT_SIZE_128_MASK             0b100
		#define ENDPOINT_SIZE_256_MASK             0b101
		#define ENDPOINT_SIZE_512_MASK             0b110
		
		#define ENDPOINT_BANK_SINGLE               0
		#define ENDPOINT_BANK_DOUBLE               (1 << EPBK0)
		
		#define ENDPOINT_CONTROLEP                 0
		#define ENDPOINT_CONTROLEP_SIZE            64
		
		#define ENDPOINT_EPNUM_MASK                0b111
		#define ENDPOINT_MAXENDPOINTS              7
		
		#define Endpoint_GetCurrentEndpoint()      (UENUM & ENDPOINT_EPNUM_MASK)
		#define Endpoint_SelectEndpoint(epnum)     UENUM = (epnum & ENDPOINT_EPNUM_MASK)
		#define Endpoint_ResetFIFO(epnum)          MACROS{ UERST  = (1 << epnum); UERST = 0; }MACROE
		#define Endpoint_EnableEndpoint()          UECONX  |=  (1 << EPEN)
		#define Endpoint_DisableEndpoint()         UECONX  &= ~(1 << EPEN)
		#define Endpoint_IsEnabled()               (UECONX &   (1 << EPEN))
		#define Endpoint_Reset(epnum)              MACROS{ Endpoint_SelectEndpoint(epnum);        \
													 Endpoint_EnableEndpoint();                   \
													 Endpoint_DisableEndpoint();    }MACROE
		#define Endpoint_AllocateMemory()          UECFG1X |=  (1 << ALLOC)
		#define Endpoint_DeallocateMemory()        UECFG1X &= ~(1 << ALLOC)
		
		#define Endpoint_ReadWriteAllowed()        (UEINTX & (1 << RWAL))

		
		#define Endpoint_ConfigureEndpoint(num, type, dir, size, banks)                           \
												   Endpoint_ConfigureEndpoint_PRV(num,            \
																				  ((type << EPTYPE0) | dir),   \
																				  ((Endpoint_BytesToEPSizeMask(size) << EPSIZE0) | banks))
		#define Endpoint_IsConfigured()            ((UESTA0X & (1 << CFGOK)) ? ENDPOINT_CONFIG_OK : ENDPOINT_CONFIG_FAIL)

	/* Inline Functions */
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

	/* Function Prototypes */
		bool Endpoint_ConfigureEndpoint_PRV(const uint8_t EndpointNum, const uint8_t UECFG0Xdata, const uint8_t UECFG1Xdata);
		void Endpoint_ClearEndpoints(void);

#endif
