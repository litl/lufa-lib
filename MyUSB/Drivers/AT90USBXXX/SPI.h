/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  SPI subsystem driver for the supported USB AVRs models.
 */

#ifndef __SPI_H__
#define __SPI_H__

	/* Includes: */
		#include <stdbool.h>

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define SPI_USE_DOUBLESPEED            (1 << 7)
	#endif
	
	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** SPI prescaler mask for SPI_Init(). Divides the system clock by a factor of 2. */
			#define SPI_SPEED_FCPU_DIV_2           SPI_USE_DOUBLESPEED

			/** SPI prescaler mask for SPI_Init(). Divides the system clock by a factor of 4. */
			#define SPI_SPEED_FCPU_DIV_4           0

			/** SPI prescaler mask for SPI_Init(). Divides the system clock by a factor of 8. */
			#define SPI_SPEED_FCPU_DIV_8           (SPI_USE_DOUBLESPEED | (1 << SPR0))

			/** SPI prescaler mask for SPI_Init(). Divides the system clock by a factor of 16. */
			#define SPI_SPEED_FCPU_DIV_16          (1 << SPR0)

			/** SPI prescaler mask for SPI_Init(). Divides the system clock by a factor of 32. */
			#define SPI_SPEED_FCPU_DIV_32          (SPI_USE_DOUBLESPEED | (1 << SPR1))

			/** SPI prescaler mask for SPI_Init(). Divides the system clock by a factor of 64. */
			#define SPI_SPEED_FCPU_DIV_64          (SPI_USE_DOUBLESPEED | (1 << SPR1) | (1 < SPR0))

			/** SPI prescaler mask for SPI_Init(). Divides the system clock by a factor of 128. */
			#define SPI_SPEED_FCPU_DIV_128         ((1 << SPR1) | (1 < SPR0))

		/* Inline Functions: */
			/** Initializes the SPI subsystem, ready for transfers. Must be called before calling any other
			 *  SPI routines.
			 *
			 *  \param PrescalerMask  Prescaler mask to set the SPI clock speed
			 *  \param Master         If true, sets the SPI system to use master mode, slave if false
			 */
			static inline void SPI_Init(const uint8_t PrescalerMask, const bool Master)
			{
				DDRB  |= ((1 << 1) | (1 << 2));
				PORTB |= ((1 << 0) | (1 << 3));
				
				SPCR   = ((1 << SPE) | (Master << MSTR) | (1 << CPOL) | (1 << CPHA) |
				          (PrescalerMask & ~SPI_USE_DOUBLESPEED));
				
				if (PrescalerMask & SPI_USE_DOUBLESPEED)
				  SPSR = (1 << SPI2X);
			}
			
			/** Sends a byte through the SPI interface, blocking until the transfer is complete.
			 *
			 *  \param Byte  Byte to send through the SPI interface
			 */
			static inline uint8_t SPI_SendByte(const uint8_t Byte)
			{
				SPDR = Byte;
				while (!(SPSR & (1 << SPIF)));
				return SPDR;
			}

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
		
#endif
