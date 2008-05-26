/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __PIPE_H__
#define __PIPE_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../../Common/Common.h"
		#include "../HighLevel/USBTask.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define PIPE_ERRORFLAG_CRC16                   (1 << 4)
			#define PIPE_ERRORFLAG_TIMEOUT                 (1 << 3)
			#define PIPE_ERRORFLAG_PID                     (1 << 2)
			#define PIPE_ERRORFLAG_DATAPID                 (1 << 1)
			#define PIPE_ERRORFLAG_DATATGL                 (1 << 0)

			#define PIPE_TOKEN_MASK                        (0b11 << PTOKEN0)
			#define PIPE_TOKEN_SETUP                       (0b00 << PTOKEN0)
			#define PIPE_TOKEN_IN                          (0b01 << PTOKEN0)
			#define PIPE_TOKEN_OUT                         (0b10 << PTOKEN0)
						
			#define PIPE_BANK_SINGLE                       0
			#define PIPE_BANK_DOUBLE                       (1 << EPBK0)
			
			#define PIPE_CONTROLPIPE                       0
			#define PIPE_CONTROLPIPE_DEFAULT_SIZE          8
			
			#define PIPE_PIPENUM_MASK                      0b111
			#define PIPE_MAXPIPES                          7
			#define PIPE_MAX_SIZE                          256

			#define PIPE_EPNUM_MASK                        0b111
			#define PIPE_EPSIZE_MASK                       0x7FF

			#define PIPE_INT_IN                            UPIENX, (1 << RXINE) , UPINTX, (1 << RXINI)
			#define PIPE_INT_OUT                           UPIENX, (1 << TXOUTE), UPINTX, (1 << TXOUTI)

			#define Pipe_BytesInPipe()                     UPBCX
			#define Pipe_ResetPipe(pipenum)        MACROS{ UPRST    =  (1 << pipenum); UPRST = 0;                  }MACROE
			#define Pipe_SelectPipe(pipenum)       MACROS{ UPNUM    =  pipenum;                                    }MACROE
			#define Pipe_GetCurrentPipe()                 (UPNUM   &   PIPE_PIPENUM_MASK)
			#define Pipe_AllocateMemory()          MACROS{ UPCFG1X |=  (1 << ALLOC);                               }MACROE
			#define Pipe_DeallocateMemory()        MACROS{ UPCFG1X &= ~(1 << ALLOC);                               }MACROE
			#define Pipe_EnablePipe()              MACROS{ UPCONX  |=  (1 << PEN);                                 }MACROE
			#define Pipe_DisablePipe()             MACROS{ UPCONX  &= ~(1 << PEN);                                 }MACROE
			#define Pipe_IsEnabled()                     ((UPCONX  &   (1 << PEN)) ? true : false)
			#define Pipe_SetToken(token)           MACROS{ UPCFG0X  = ((UPCFG0X & ~PIPE_TOKEN_MASK) | token);      }MACROE
			
			#define Pipe_SetInfiniteINRequests()   MACROS{ UPCONX  |=  (1 << INMODE);                              }MACROE
			#define Pipe_SetFiniteINRequests(n)    MACROS{ UPCONX  &= ~(1 << INMODE); UPINRQX = n;                 }MACROE
			
			#define Pipe_ConfigurePipe(num, type, token, epnum, size, banks)                 \
												   MACROS{ Pipe_ConfigurePipe_P(num,         \
																			  ((type << PTYPE0) | token | ((epnum & PIPE_EPNUM_MASK) << PEPNUM0)),   \
																			  (Pipe_BytesToEPSizeMask(size) | banks)); }MACROE
			#define Pipe_IsConfigured()                  ((UPSTAX  & (1 << CFGOK)) ? true : false)
			#define Pipe_SetInterruptFreq(ms)      MACROS{ UPCFG2X  = ms;                                          }MACROE
			#define Pipe_GetPipeInterrupts()               UPINT
			#define Pipe_ClearPipeInterrupt(n)     MACROS{ UPINT   &= ~(1 << n);                                   }MACROE
			#define Pipe_HasPipeInterrupted(n)           ((UPINT   &   (1 << n)) ? true : false)
			#define Pipe_FIFOCON_Clear()           MACROS{ UPINTX  &= ~(1 << FIFOCON);                             }MACROE
			#define Pipe_Unfreeze()                MACROS{ UPCONX  &= ~(1 << PFREEZE);                             }MACROE
			#define Pipe_Freeze()                  MACROS{ UPCONX  |=  (1 << PFREEZE);                             }MACROE

			#define Pipe_ClearError()              MACROS{ UPINTX  &= ~(1 << PERRI);                               }MACROE
			#define Pipe_IsError()                       ((UPINTX  &   (1 << PERRI)) ? true : false)
			#define Pipe_ClearErrorFlags()         MACROS{ UPERRX   = 0;                                           }MACROE
			#define Pipe_GetErrorFlags()                   UPERRX

			#define Pipe_ReadWriteAllowed()              ((UPINTX  & (1 << RWAL)) ? true : false)

			#define Pipe_ClearSetupSent()          MACROS{ UPINTX  &= ~(1 << TXSTPI);                              }MACROE
			#define Pipe_IsSetupSent()                   ((UPINTX  &   (1 << TXSTPI)) ? true : false)
			#define Pipe_ClearStall()              MACROS{ UPINTX  &= ~(1 << RXSTALLI);                            }MACROE             
			#define Pipe_IsStalled()                     ((UPINTX  &   (1 << RXSTALLI)) ? true : false)

			#define Pipe_IsSetupINReceived()             ((UPINTX  &   (1 << RXINI)) ? true : false)
			#define Pipe_IsSetupOUTReady()               ((UPINTX  &   (1 << TXOUTI)) ? true : false)
			#define Pipe_ClearSetupIN()            MACROS{ UPINTX  &= ~(1 << RXINI); UPINTX &= ~(1 << FIFOCON);    }MACROE
			#define Pipe_ClearSetupOUT()           MACROS{ UPINTX  &= ~(1 << TXOUTI); UPINTX &= ~(1 << FIFOCON);   }MACROE

		/* Enums: */
			enum Pipe_Stream_RW_ErrorCodes_t
			{
				PIPE_RWSTREAM_ERROR_NoError            = 0,
				PIPE_RWSTREAM_ERROR_PipeStalled        = 1,				
				PIPE_RWSTREAM_ERROR_DeviceDisconnected = 2,
			};

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
				uint8_t Dummy;
				
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
				uint8_t Dummy;
				
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
				uint8_t Dummy;
				
				Dummy = UPDATX;
				Dummy = UPDATX;
				Dummy = UPDATX;
				Dummy = UPDATX;
			}

		/* Function Aliases: */
			#define Pipe_Read_Word()                   Pipe_Read_Word_LE()
			#define Pipe_Write_Word(Word)              Pipe_Write_Word_LE(Word)
			#define Pipe_Read_DWord()                  Pipe_Read_DWord_LE()
			#define Pipe_Write_DWord(DWord)            Pipe_Write_DWord_LE(DWord)
			#define Pipe_Write_Stream(Data, Length)    Pipe_Write_Stream_LE(Data, Length)
			#define Pipe_Read_Stream(Buffer, Length)   Pipe_Read_Stream_LE(Buffer, Length)

		/* External Variables: */
			extern uint8_t USB_ControlPipeSize;

		/* Function Prototypes: */
			uint8_t Pipe_Write_Stream_LE(void* Data, uint16_t Length) ATTR_NON_NULL_PTR_ARG(1);
			uint8_t Pipe_Write_Stream_BE(void* Data, uint16_t Length) ATTR_NON_NULL_PTR_ARG(1);
			uint8_t Pipe_Read_Stream_LE(void* Data, uint16_t Length)  ATTR_NON_NULL_PTR_ARG(1);
			uint8_t Pipe_Read_Stream_BE(void* Data, uint16_t Length)  ATTR_NON_NULL_PTR_ARG(1);
		
	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Inline Functions: */
			static inline uint8_t Pipe_BytesToEPSizeMask(uint16_t Bytes)
			                                             ATTR_WARN_UNUSED_RESULT ATTR_CONST;
			static inline uint8_t Pipe_BytesToEPSizeMask(uint16_t Bytes)
			{
				uint8_t SizeCheck = 8;
				uint8_t SizeMask  = 0;
				
				Bytes &= PIPE_EPSIZE_MASK;

				do
				{
					if (Bytes <= SizeCheck)
					  return (SizeMask << EPSIZE0);
					
					SizeCheck <<= 1;
					SizeMask++;
				} while (SizeCheck != (PIPE_MAX_SIZE >> 1));
				
				return (SizeMask + 1);
			};
		
		/* Function Prototypes: */
			void Pipe_ClearPipes(void);
			void Pipe_ConfigurePipe_P(const uint8_t PipeNum, const uint8_t UPCFG0Xdata,
			                          const uint8_t UPCFG1Xdata);
	#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
		
#endif
