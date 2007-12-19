/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../../Common/Common.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define ENDPOINT_CONFIG_OK                         true
			#define ENDPOINT_CONFIG_FAIL                       false

			#define ENDPOINT_DIR_OUT                           0
			#define ENDPOINT_DIR_IN                            (1 << EPDIR)
			
			#define ENDPOINT_SIZE_8_MASK                       (0b000 << EPSIZE0)
			#define ENDPOINT_SIZE_16_MASK                      (0b001 << EPSIZE0)
			#define ENDPOINT_SIZE_32_MASK                      (0b010 << EPSIZE0)
			#define ENDPOINT_SIZE_64_MASK                      (0b011 << EPSIZE0)
			#define ENDPOINT_SIZE_128_MASK                     (0b100 << EPSIZE0)
			#define ENDPOINT_SIZE_256_MASK                     (0b101 << EPSIZE0)
			#define ENDPOINT_SIZE_512_MASK                     (0b110 << EPSIZE0)
			
			#define ENDPOINT_BANK_SINGLE                       0
			#define ENDPOINT_BANK_DOUBLE                       (1 << EPBK0)
			
			#define ENDPOINT_CONTROLEP                         0
			#define ENDPOINT_CONTROLEP_SIZE                    64
			
			#define ENDPOINT_EPNUM_MASK                        0b111
			#define ENDPOINT_MAXENDPOINTS                      7

			#define ENDPOINT_INT_IN                            UEIENX, (1 << TXINE) , UPINTX, (1 << TXINI)
			#define ENDPOINT_INT_OUT                           UEIENX, (1 << RXOUTE), UPINTX, (1 << RXOUTI)
			
			#define Endpoint_BytesInEndpoint()                 UEBCX
			#define Endpoint_GetCurrentEndpoint()             (UENUM   & ENDPOINT_EPNUM_MASK)
			#define Endpoint_SelectEndpoint(epnum)     MACROS{ UENUM    =  (epnum & ENDPOINT_EPNUM_MASK); }MACROE
			#define Endpoint_ResetFIFO(epnum)          MACROS{ UERST    =  (1 << (epnum & ENDPOINT_EPNUM_MASK)); UERST = 0;       }MACROE
			#define Endpoint_EnableEndpoint()          MACROS{ UECONX  |=  (1 << EPEN);                  }MACROE
			#define Endpoint_DisableEndpoint()         MACROS{ UECONX  &= ~(1 << EPEN);                  }MACROE
			#define Endpoint_IsEnabled()                     ((UECONX  &   (1 << EPEN)) ? true : false)
			#define Endpoint_ResetEndpoint(epnum)      MACROS{ UENUM    =  (epnum & ENDPOINT_EPNUM_MASK); uint8_t temp = UECONX; UECONX = (temp | (1 << EPEN)); UECONX = (temp & ~(1 << EPEN)); }MACROE
			#define Endpoint_AllocateMemory()          MACROS{ UECFG1X |=  (1 << ALLOC);                }MACROE
			#define Endpoint_DeallocateMemory()        MACROS{ UECFG1X &= ~(1 << ALLOC);                }MACROE
			
			#define Endpoint_ReadWriteAllowed()              ((UEINTX  & (1 << RWAL)) ? true : false)

			
			#define Endpoint_ConfigureEndpoint(num, type, dir, size, banks)                           \
												       Endpoint_ConfigureEndpoint_P(num,              \
                                                       ((type << EPTYPE0) | dir),                     \
											           (Endpoint_BytesToEPSizeMask(size) | banks))
			#define Endpoint_IsConfigured()                  ((UESTA0X & (1 << CFGOK)) ? ENDPOINT_CONFIG_OK : ENDPOINT_CONFIG_FAIL)
			#define Endpoint_GetEndpointInterrupts()           UEINT
			#define Endpoint_ClearEndpointInterrupt(n) MACROS{ UEINT   &= ~(1 << n);                     }MACROE
			#define Endpoint_HasEndpointInterrupted(n)       ((UEINT   & (1 << (n & ENDPOINT_EPNUM_MASK))) ? true : false)

			#define Endpoint_FIFOCON_Clear()           MACROS{ UEINTX  &= ~(1 << FIFOCON);              }MACROE
			
			#define Endpoint_In_Clear()                MACROS{ UEINTX  &= ~(1 << TXINI); UEINTX &= ~(1 << FIFOCON); }MACROE
			#define Endpoint_Out_Clear()               MACROS{ UEINTX  &= ~(1 << RXOUTI); UEINTX &= ~(1 << FIFOCON); }MACROE

			#define Endpoint_In_IsReady()                    ((UEINTX  &   (1 << TXINI)) ? true : false)
			#define Endpoint_Out_IsReceived()                ((UEINTX  &   (1 << RXOUTI)) ? true: false)
			
			#define Endpoint_ClearSetupReceived()      MACROS{ UEINTX  &= ~(1 << RXSTPI);               }MACROE
			#define Endpoint_Stall_Transaction()       MACROS{ UECONX  |=  (1 << STALLRQ);              }MACROE
			#define Endpoint_ClearStall()              MACROS{ UECONX  &= ~(1 << STALLRQ);              }MACROE
			#define Endpoint_IsStalled()                     ((UECONX  &   (1 << STALLRQ)) ? true : false)

			#define Endpoint_ResetDataToggle()         MACROS{ UECONX  |= (1 << RSTDT);                  }MACROE

		/* Function Aliases */
			static inline uint16_t Endpoint_Read_Word(void)            ATTR_ALIAS(Endpoint_Read_Word_LE) ATTR_WARN_UNUSED_RESULT;
			static inline void     Endpoint_Write_Word(uint16_t Word)  ATTR_ALIAS(Endpoint_Write_Word_LE);
			static inline uint32_t Endpoint_Read_DWord(void)           ATTR_ALIAS(Endpoint_Read_DWord_LE) ATTR_WARN_UNUSED_RESULT;
			static inline void     Endpoint_Write_DWord(uint32_t Word) ATTR_ALIAS(Endpoint_Write_DWord_LE);
			
		/* Inline Functions: */
			static inline uint8_t Endpoint_Read_Byte(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Endpoint_Read_Byte(void)
			{
				return UEDATX;
			}

			static inline void Endpoint_Write_Byte(const uint8_t Byte)
			{
				UEDATX = Byte;
			}

			static inline void Endpoint_Ignore_Byte(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UEDATX;
			}
			
			static inline uint16_t Endpoint_Read_Word_LE(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint16_t Endpoint_Read_Word_LE(void)
			{
				uint16_t Data;
				
				Data  = UEDATX;
				Data |= (((uint16_t)UEDATX) << 8);
			
				return Data;
			}

			static inline uint16_t Endpoint_Read_Word_BE(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint16_t Endpoint_Read_Word_BE(void)
			{
				uint16_t Data;
				
				Data  = (((uint16_t)UEDATX) << 8);
				Data |= UEDATX;
			
				return Data;
			}

			static inline void Endpoint_Write_Word_LE(const uint16_t Word)
			{
				UEDATX = (Word & 0xFF);
				UEDATX = (Word >> 8);
			}
			
			static inline void Endpoint_Write_Word_BE(const uint16_t Word)
			{
				UEDATX = (Word >> 8);
				UEDATX = (Word & 0xFF);
			}

			static inline void Endpoint_Ignore_Word(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UEDATX;
				Dummy = UEDATX;
			}

			static inline uint32_t Endpoint_Read_DWord_LE(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint32_t Endpoint_Read_DWord_LE(void)
			{
				union
				{
					uint32_t DWord;
					uint8_t  Bytes[4];
				} Data;
				
				Data.Bytes[0] = UEDATX;
				Data.Bytes[1] = UEDATX;
				Data.Bytes[2] = UEDATX;
				Data.Bytes[3] = UEDATX;
			
				return Data.DWord;
			}

			static inline uint32_t Endpoint_Read_DWord_BE(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint32_t Endpoint_Read_DWord_BE(void)
			{
				union
				{
					uint32_t DWord;
					uint8_t  Bytes[4];
				} Data;
				
				Data.Bytes[3] = UEDATX;
				Data.Bytes[2] = UEDATX;
				Data.Bytes[1] = UEDATX;
				Data.Bytes[0] = UEDATX;
			
				return Data.DWord;
			}

			static inline void Endpoint_Write_DWord_LE(const uint32_t DWord)
			{
				Endpoint_Write_Word_LE(DWord);
				Endpoint_Write_Word_LE(DWord >> 16);
			}
			
			static inline void Endpoint_Write_DWord_BE(const uint32_t DWord)
			{
				Endpoint_Write_Word_BE(DWord >> 16);
				Endpoint_Write_Word_BE(DWord);
			}

			static inline void Endpoint_Ignore_DWord(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UEDATX;
				Dummy = UEDATX;
				Dummy = UEDATX;
				Dummy = UEDATX;
			}

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
