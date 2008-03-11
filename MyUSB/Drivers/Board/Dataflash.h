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
	#include "../../Common/BoardTypes.h"
	
	#if defined(BOARD) && (BOARD == BOARD_USBKEY)
		#include "USBKEY/Dataflash.h"
	#elif defined(BOARD) && (BOARD == BOARD_STK525)
		#include "STK525/Dataflash.h"
	#elif defined(BOARD) && (BOARD == BOARD_STK526)
		#include "STK526/Dataflash.h"
	#else
		#error BOARD must be set in makefile to BOARD_USBKEY, BOARD_STK525 or BOARD_STK526.
	#endif
	
	/* Public Interface - May be used in end-application: */
		/* Function Prototypes: */
			void Dataflash_SelectChipFromPage(const uint16_t PageAddress);
			void Dataflash_SendAddressBytes(uint16_t PageAddress, const uint16_t BufferByte);
			void Dataflash_ToggleSelectedChipCS(void);

		/* Inline Functions: */
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
