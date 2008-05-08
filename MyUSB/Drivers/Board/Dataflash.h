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
	#include "../AT90USBXXX/SPI.h"
	#include "../../Common/Common.h"
	#include "../../Common/BoardTypes.h"
	
	#if !defined(BOARD)
		#error BOARD must be set in makefile to BOARD_USBKEY, BOARD_STK525, BOARD_STK526, BOARD_RZUSBSTICK or BOARD_USER.	
	#elif (BOARD == BOARD_USBKEY)
		#include "USBKEY/Dataflash.h"
	#elif (BOARD == BOARD_STK525)
		#include "STK525/Dataflash.h"
	#elif (BOARD == BOARD_STK526)
		#include "STK526/Dataflash.h"
	#elif (BOARD == BOARD_RZUSBSTICK)
		#error The selected board does not contain a Dataflash IC.
	#elif (BOARD == BOARD_USER)
		#include "Board/Dataflash.h"
	#endif
	
	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define Dataflash_GetSelectedChip()          (DATAFLASH_CHIPCS_PORT & DATAFLASH_CHIPCS_MASK)
			#define Dataflash_SelectChip(mask)   MACROS{ DATAFLASH_CHIPCS_PORT = ((DATAFLASH_CHIPCS_PORT \
			                                             & ~DATAFLASH_CHIPCS_MASK) | mask);              }MACROE
			#define Dataflash_DeselectChip()             Dataflash_SelectChip(DATAFLASH_NO_CHIP)

		/* Inline Functions: */
			static inline void Dataflash_Init(const uint8_t PrescalerMask)
			{
				DATAFLASH_CHIPCS_DDR  |= DATAFLASH_CHIPCS_MASK;
				DATAFLASH_CHIPCS_PORT |= DATAFLASH_CHIPCS_MASK;

				SPI_Init(PrescalerMask, true);
			}

			static inline uint8_t Dataflash_SendByte(const uint8_t Byte)
			{
				return SPI_SendByte(Byte);
			}

			static inline void Dataflash_ToggleSelectedChipCS(void)
			{
				#if (DATAFLASH_TOTALCHIPS == 2)
					uint8_t SelectedChipMask = Dataflash_GetSelectedChip();
					
					Dataflash_SelectChip(DATAFLASH_NO_CHIP);
					Dataflash_SelectChip(SelectedChipMask);
				#else
					Dataflash_SelectChip(DATAFLASH_NO_CHIP);
					Dataflash_SelectChip(DATAFLASH_CHIP1);	
				#endif
			}

			static inline void Dataflash_WaitWhileBusy(void)
			{
				Dataflash_ToggleSelectedChipCS();			
				Dataflash_SendByte(DF_CMD_GETSTATUS);
				while (!(Dataflash_SendByte(0x00) & DF_STATUS_READY));
			}

			static inline void Dataflash_SelectChipFromPage(const uint16_t PageAddress)
			{
				if (PageAddress > (DATAFLASH_PAGES * DATAFLASH_TOTALCHIPS))
				{
					Dataflash_SelectChip(DATAFLASH_NO_CHIP);
					return;
				}

				#if (DATAFLASH_TOTALCHIPS == 2)
					if (PageAddress & 0x01)
					  Dataflash_SelectChip(DATAFLASH_CHIP2);
					else
					  Dataflash_SelectChip(DATAFLASH_CHIP1);
				#else
					Dataflash_SelectChip(DATAFLASH_NO_CHIP);
					Dataflash_SelectChip(DATAFLASH_CHIP1);
				#endif
			}

			static inline void Dataflash_SendAddressBytes(uint16_t PageAddress, const uint16_t BufferByte)
			{	
				#if (DATAFLASH_TOTALCHIPS == 2)
					PageAddress >>= 1;
				#endif

				Dataflash_SendByte(PageAddress >> 5);
				Dataflash_SendByte((PageAddress << 3) | (BufferByte >> 8));
				Dataflash_SendByte(BufferByte);
			}

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
	
#endif
