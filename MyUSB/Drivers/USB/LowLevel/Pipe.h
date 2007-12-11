/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __PIPE_H__
#define __PIPE_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../../Common/FunctionAttributes.h"
		#include "../../../Common/Common.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define PIPE_CONFIG_OK                         true
			#define PIPE_CONFIG_FAIL                       false

			#define PIPE_ERRORFLAG_CRC16                   (1 << 4)
			#define PIPE_ERRORFLAG_TIMEOUT                 (1 << 3)
			#define PIPE_ERRORFLAG_PID                     (1 << 2)
			#define PIPE_ERRORFLAG_DATAPID                 (1 << 1)
			#define PIPE_ERRORFLAG_DATATGL                 (1 << 0)

			#define PIPE_TOKEN_MASK                        (0x03 << PTOKEN0)
			#define PIPE_TOKEN_SETUP                       0b00
			#define PIPE_TOKEN_IN                          0b01
			#define PIPE_TOKEN_OUT                         0b10
			
			#define PIPE_SIZE_8_MASK                       0b000
			#define PIPE_SIZE_16_MASK                      0b001
			#define PIPE_SIZE_32_MASK                      0b010
			#define PIPE_SIZE_64_MASK                      0b011
			#define PIPE_SIZE_128_MASK                     0b100
			#define PIPE_SIZE_256_MASK                     0b101
			#define PIPE_SIZE_512_MASK                     0b110
			#define PIPE_SIZE_1024_MASK                    0b111
			
			#define PIPE_BANK_SINGLE                       0
			#define PIPE_BANK_DOUBLE                       (1 << EPBK0)
			
			#define PIPE_CONTROLPIPE                       0
			#define PIPE_CONTROLPIPE_DEFAULT_SIZE          8
			
			#define PIPE_PIPENUM_MASK                      0b111
			#define PIPE_MAXPIPES                          7

			#define PIPE_EPNUM_MASK                        0b111

			#define PIPE_INT_IN                            UPIENX, (1 << RXINE) , UPINTX, (1 << RXINI)
			#define PIPE_INT_OUT                           UPIENX, (1 << TXOUTE), UPINTX, (1 << TXOUTI)

			#define Pipe_BytesInPipe()                     UPBCX
			#define Pipe_ResetPipe(pipenum)        MACROS{ UPRST    =  (1 << pipenum); UPRST = 0;                }MACROE
			#define Pipe_SelectPipe(pipenum)       MACROS{ UPNUM    =  (pipenum & PIPE_PIPENUM_MASK);            }MACROE
			#define Pipe_AllocateMemory()          MACROS{ UPCFG1X |=  (1 << ALLOC);                             }MACROE
			#define Pipe_DeallocateMemory()        MACROS{ UPCFG1X &= ~(1 << ALLOC);                             }MACROE
			#define Pipe_EnablePipe()              MACROS{ UPCONX  |=  (1 << PEN);                               }MACROE
			#define Pipe_DisablePipe()             MACROS{ UPCONX  &= ~(1 << PEN);                               }MACROE
			#define Pipe_IsEnabled()                     ((UPCONX  &   (1 << PEN)) ? true : false)
			#define Pipe_SetToken(token)           MACROS{ UPCFG0X  = ((UPCFG0X & ~PIPE_TOKEN_MASK) | (token << PTOKEN0)); }MACROE
			
			#define Pipe_SetInfiniteINRequests()   MACROS{ UPCONX  |=  (1 << INMODE);                            }MACROE
			#define Pipe_SetFiniteINRequests(n)    MACROS{ UPCONX  &= ~(1 << INMODE); UPINRQX = n;               }MACROE
			
			#define Pipe_ConfigurePipe(num, type, token, epnum, size, banks)                 \
												   MACROS{ Pipe_ConfigurePipe_P(num,         \
																			  ((type << PTYPE0) | (token << PTOKEN0) | ((epnum & PIPE_EPNUM_MASK) << PEPNUM0)),   \
																			  ((Pipe_BytesToEPSizeMask(size) << EPSIZE0) | banks)); }MACROE
			#define Pipe_IsConfigured()                  ((UPSTAX  & (1 << CFGOK)) ? PIPE_CONFIG_OK : PIPE_CONFIG_FAIL)
			#define Pipe_SetInterruptFreq(ms)      MACROS{ UPCFG2X  = ms;                                          }MACROE
			#define Pipe_GetPipeInterrupts()               UPINT
			#define Pipe_ClearPipeInterrupt(n)     MACROS{ UPINT   &= ~(1 << n);                                   }MACROE
			#define Pipe_HasPipeInterrupted(n)           ((UPINT   &   (1 << n)) ? true : false)
			#define Pipe_ResetFIFO()               MACROS{ UPINTX  &= ~(1 << FIFOCON);                             }MACROE
			#define Pipe_Unfreeze()                MACROS{ UPCONX  &= ~(1 << PFREEZE);                             }MACROE
			#define Pipe_Freeze()                  MACROS{ UPCONX  |=  (1 << PFREEZE);                             }MACROE

			#define Pipe_ClearError()              MACROS{ UPINTX  &= ~(1 << PERRI);                              }MACROE
			#define Pipe_IsError()                       ((UPINTX  &   (1 << PERRI)) ? true : false)
			#define Pipe_ClearErrorFlags()         MACROS{ UPERRX   = 0;                                            }MACROE
			#define Pipe_GetErrorFlags()                   UPERRX

			#define Pipe_ClearSetupSent()          MACROS{ UPINTX  &= ~(1 << TXSTPI);                             }MACROE
			#define Pipe_IsSetupSent()                   ((UPINTX  &   (1 << TXSTPI)) ? true : false)
			#define Pipe_ClearStall()              MACROS{ UPINTX  &= ~(1 << RXSTALLI);                           }MACROE             
			#define Pipe_IsStalled()                     ((UPINTX  &   (1 << RXSTALLI)) ? true : false)

			#define Pipe_In_Clear()                MACROS{ UPINTX  &= ~(1 << RXINI); UPINTX &= ~(1 << FIFOCON);   }MACROE
			#define Pipe_In_IsReceived()                 ((UPINTX  &   (1 << RXINI)) ? true : false)
			#define Pipe_Out_Clear()               MACROS{ UPINTX  &= ~(1 << TXOUTI); UPINTX &= ~(1 << FIFOCON);  }MACROE
			#define Pipe_Out_IsReady()                   ((UPINTX  &   (1 << TXOUTI)) ? true : false)

		/* Function Aliases */
			static inline uint16_t Pipe_Read_Word(void)            ATTR_ALIAS(Pipe_Read_Word_LE) ATTR_WARN_UNUSED_RESULT;
			static inline void     Pipe_Write_Word(uint16_t Word)  ATTR_ALIAS(Pipe_Write_Word_LE);
			static inline uint32_t Pipe_Read_DWord(void)           ATTR_ALIAS(Pipe_Read_DWord_LE) ATTR_WARN_UNUSED_RESULT;
			static inline void     Pipe_Write_DWord(uint32_t Word) ATTR_ALIAS(Pipe_Write_DWord_LE);

		/* Inline Functions: */
			static inline uint8_t Pipe_Read_Byte(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Pipe_Read_Byte(void)
			{
				return UPDATX;
			}

			static inline void Pipe_Write_Byte(const uint8_t Byte)
			{
				UPDATX = Byte;
			}

			static inline void Pipe_Ignore_Byte(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UPDATX;
			}
			
			static inline uint16_t Pipe_Read_Word_LE(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint16_t Pipe_Read_Word_LE(void)
			{
				uint16_t Data;
				
				Data  = UPDATX;
				Data |= (((uint16_t)UPDATX) << 8);
			
				return Data;
			}

			static inline uint16_t Pipe_Read_Word_BE(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint16_t Pipe_Read_Word_BE(void)
			{
				uint16_t Data;
				
				Data  = (((uint16_t)UPDATX) << 8);
				Data |= UPDATX;
			
				return Data;
			}
			
			static inline void Pipe_Write_Word_LE(const uint16_t Word)
			{
				UPDATX = (Word & 0xFF);
				UPDATX = (Word >> 8);
			}
			
			static inline void Pipe_Write_Word_BE(const uint16_t Word)
			{
				UPDATX = (Word >> 8);
				UPDATX = (Word & 0xFF);
			}

			static inline void Pipe_Ignore_Word(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UPDATX;
				Dummy = UPDATX;
			}

			static inline uint32_t Pipe_Read_DWord_LE(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint32_t Pipe_Read_DWord_LE(void)
			{
				union
				{
					uint32_t DWord;
					uint8_t  Bytes[4];
				} Data;
				
				Data.Bytes[0] = UPDATX;
				Data.Bytes[1] = UPDATX;
				Data.Bytes[2] = UPDATX;
				Data.Bytes[3] = UPDATX;
			
				return Data.DWord;
			}

			static inline uint32_t Pipe_Read_DWord_BE(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint32_t Pipe_Read_DWord_BE(void)
			{
				union
				{
					uint32_t DWord;
					uint8_t  Bytes[4];
				} Data;
				
				Data.Bytes[3] = UPDATX;
				Data.Bytes[2] = UPDATX;
				Data.Bytes[1] = UPDATX;
				Data.Bytes[0] = UPDATX;
			
				return Data.DWord;
			}

			static inline void Pipe_Write_DWord_LE(const uint32_t DWord)
			{
				Pipe_Write_Word_LE(DWord);
				Pipe_Write_Word_LE(DWord >> 16);
			}
			
			static inline void Pipe_Write_DWord_BE(const uint32_t DWord)
			{
				Pipe_Write_Word_BE(DWord >> 16);
				Pipe_Write_Word_BE(DWord);
			}			
			
			static inline void Pipe_Ignore_DWord(void)
			{
				volatile uint8_t Dummy;
				
				Dummy = UPDATX;
				Dummy = UPDATX;
				Dummy = UPDATX;
				Dummy = UPDATX;
			}

		/* External Variables: */
			uint8_t USB_ControlPipeSize;

		/* Function Prototypes: */
			void Pipe_ClearPipes(void);
		
	/* Private Interface - For use in library only: */
		/* Inline Functions: */
			static inline uint8_t Pipe_BytesToEPSizeMask(const uint16_t Bytes)
			                                             ATTR_WARN_UNUSED_RESULT ATTR_CONST;
			static inline uint8_t Pipe_BytesToEPSizeMask(const uint16_t Bytes)
			{
				if (Bytes <= 8)
				  return PIPE_SIZE_8_MASK;
				else if (Bytes <= 16)
				  return PIPE_SIZE_16_MASK;
				else if (Bytes <= 32)
				  return PIPE_SIZE_32_MASK;
				else if (Bytes <= 64)
				  return PIPE_SIZE_64_MASK;
				else if (Bytes <= 128)
				  return PIPE_SIZE_128_MASK;
				else if (Bytes <= 256)
				  return PIPE_SIZE_256_MASK;
				else if (Bytes <= 512)
				  return PIPE_SIZE_512_MASK;
				else
				  return PIPE_SIZE_1024_MASK;			
			};
		
		/* Function Prototypes: */
			bool    Pipe_ConfigurePipe_P(const uint8_t PipeNum, const uint8_t UPCFG0Xdata, const uint8_t UPCFG1Xdata);

#endif
