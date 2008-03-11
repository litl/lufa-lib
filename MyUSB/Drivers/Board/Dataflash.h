/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __DATAFLASH_H__
#define __DATAFLASH_H__

	/* Macros: */
	#define INCLUDE_FROM_DATAFLASH_H
	#define INCLUDE_FROM_BOARD_DRIVER

	/* Includes: */
	#include "../../Common/Common.h"
	#include "../../Common/BoardTypes.h"
	
	#if !defined(BOARD)
		#error BOARD must be set in makefile to BOARD_USBKEY, BOARD_STK525, BOARD_STK526 or BOARD_USER.	
	#elif (BOARD == BOARD_USBKEY)
		#include "USBKEY/Dataflash.h"
	#elif (BOARD == BOARD_STK525)
		#include "STK525/Dataflash.h"
	#elif (BOARD == BOARD_STK526)
		#include "STK526/Dataflash.h"
	#elif (BOARD == BOARD_USER)
		#include "Board/Dataflash.h"
	#endif
	
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define DATAFLASH_USE_DOUBLESPEED            (1 << 7)

			#define DATAFLASH_SPEED_FCPU_DIV_2           DATAFLASH_USE_DOUBLESPEED
			#define DATAFLASH_SPEED_FCPU_DIV_4           0
			#define DATAFLASH_SPEED_FCPU_DIV_8           (DATAFLASH_USE_DOUBLESPEED | (1 << SPR0))
			#define DATAFLASH_SPEED_FCPU_DIV_16          (1 << SPR0)
			#define DATAFLASH_SPEED_FCPU_DIV_32          (DATAFLASH_USE_DOUBLESPEED | (1 << SPR1))
			#define DATAFLASH_SPEED_FCPU_DIV_64          (DATAFLASH_USE_DOUBLESPEED | (1 << SPR1) | (1 < SPR0))
			#define DATAFLASH_SPEED_FCPU_DIV_128         ((1 << SPR1) | (1 < SPR0))

			#define Dataflash_GetSelectedChip()          (DATAFLASH_CHIPCS_PORT & DATAFLASH_CHIPCS_MASK)
			#define Dataflash_SelectChip(mask)   MACROS{ DATAFLASH_CHIPCS_PORT = ((DATAFLASH_CHIPCS_PORT \
			                                             & ~DATAFLASH_CHIPCS_MASK) | mask);              }MACROE
			#define Dataflash_DeselectChip()             Dataflash_SelectChip(DATAFLASH_NO_CHIP)

		/* Function Prototypes: */
			void Dataflash_SelectChipFromPage(const uint16_t PageAddress);
			void Dataflash_SendAddressBytes(uint16_t PageAddress, const uint16_t BufferByte);
			void Dataflash_ToggleSelectedChipCS(void);

		/* Inline Functions: */
			static inline void Dataflash_Init(const uint8_t PrescalerMask)
			{
				DDRB  |= ((1 << 1) | (1 << 2));
				PORTB |= ((1 << 0) | (1 << 3));
				
				DATAFLASH_CHIPCS_DDR  |= DATAFLASH_CHIPCS_MASK;
				DATAFLASH_CHIPCS_PORT |= DATAFLASH_CHIPCS_MASK;

				SPCR   = ((1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) |
				          (PrescalerMask & ~DATAFLASH_USE_DOUBLESPEED));
				
				if (PrescalerMask & DATAFLASH_USE_DOUBLESPEED)
				  SPSR = (1 << SPI2X);
			}

			static inline uint8_t Dataflash_SendByte(const uint8_t Byte)
			{
				SPDR = Byte;
				while (!(SPSR & (1 << SPIF)));
				return SPDR;
			}

			static inline void Dataflash_WaitWhileBusy(void)
			{
				Dataflash_ToggleSelectedChipCS();			
				Dataflash_SendByte(DF_CMD_GETSTATUS);
				while (!(Dataflash_SendByte(0x00) & DF_STATUS_READY));
			}	

#endif
