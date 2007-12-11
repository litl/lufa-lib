/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __DATAFLASH_H__
#define __DATAFLASH_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		
		#include "../../Common/Common.h"
		#include "../Misc/DataflashCommandBytes.h"
		
	/* Private Interface - For use in library only: */
		/* Macros */
			#define DATAFLASH_CHIPCS_MASK        (DATAFLASH_CHIP1 | DATAFLASH_CHIP2)

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define DATAFLASH_NO_CHIP            DATAFLASH_CHIPCS_MASK
			#define DATAFLASH_CHIP1              (1 << 1)
			#define DATAFLASH_CHIP2              (1 << 0)
			
			#define DATAFLASH_PAGE_SIZE          1024
			#define DATAFLASH_PAGES              8192
			
			#define DATAFLASH_SPEED_FCPU_DIV_2   (1 << 7)
			#define DATAFLASH_SPEED_FCPU_DIV_4   0
			#define DATAFLASH_SPEED_FCPU_DIV_8   ((1 << 7) | (1 << SPR0))
			#define DATAFLASH_SPEED_FCPU_DIV_16  (1 << SPR0)
			#define DATAFLASH_SPEED_FCPU_DIV_32  ((1 << 7) | (1 << SPR1))
			#define DATAFLASH_SPEED_FCPU_DIV_64  ((1 << 7) | (1 << SPR1) | (1 < SPR0))
			#define DATAFLASH_SPEED_FCPU_DIV_128 ((1 << SPR1) | (1 < SPR0))
					
			#define Dataflash_GetSelectedChip()         (PORTE & DATAFLASH_CHIPCS_MASK)
			#define Dataflash_SelectChip(mask)   MACROS{ PORTE = ((PORTE & ~DATAFLASH_CHIPCS_MASK) | mask); }MACROE
			#define Dataflash_DeselectChip()     Dataflash_SelectChip(DATAFLASH_NO_CHIP)

		/* Function Prototypes: */
			void    Dataflash_SelectChipFromPage(const uint16_t PageAddress);
			void    Dataflash_SendAddressBytes(uint16_t PageAddress, const uint16_t BufferByte);
			void    Dataflash_ToggleSelectedChipCS(void);
			
		/* Inline Functions: */
			static inline void Dataflash_Init(const uint8_t Speed)
			{
				PINB  |= (1 << 0);
				DDRB  |= ((1 << 1) | (1 << 2));
				DDRE  |= DATAFLASH_CHIPCS_MASK;
				PORTE |= DATAFLASH_CHIPCS_MASK;

				SPCR  = ((1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) | (Speed & ~(1 << 7)));

				if (Speed & (1 << 7))
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
