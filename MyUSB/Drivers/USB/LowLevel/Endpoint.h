/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
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
		#include "../HighLevel/USBTask.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define ENDPOINT_DIR_OUT                           0
			#define ENDPOINT_DIR_IN                            (1 << EPDIR)

			#define ENDPOINT_BANK_SINGLE                       0
			#define ENDPOINT_BANK_DOUBLE                       (1 << EPBK0)
			
			#define ENDPOINT_CONTROLEP                         0
			#define ENDPOINT_CONTROLEP_DEFAULT_SIZE            8
			
			#define ENDPOINT_EPNUM_MASK                        0b111
			
			#if defined(USB_FULL_CONTROLLER)
				#define ENDPOINT_MAXENDPOINTS                  7
				#define ENDPOINT_MAX_SIZE                      256
			#else
				#define ENDPOINT_MAXENDPOINTS                  5			
				#define ENDPOINT_MAX_SIZE                      64			
			#endif

			#define ENDPOINT_INT_SETUP                         UEIENX, (1 << RXSTPE), UEINTX, (1 << RXSTPI)
			#define ENDPOINT_INT_IN                            UEIENX, (1 << TXINE) , UEINTX, (1 << TXINI)
			#define ENDPOINT_INT_OUT                           UEIENX, (1 << RXOUTE), UEINTX, (1 << RXOUTI)
			
			#if defined(USB_FULL_CONTROLLER)
				#if defined(__AVR_ATmega32U4__)
					#define Endpoint_BytesInEndpoint()         (((uint16_t)UEBCHX << 8) | UEBCLX)
				#else
					#define Endpoint_BytesInEndpoint()         UEBCX
				#endif
			#else
				#define Endpoint_BytesInEndpoint()             UEBCLX
			#endif
			
			#define Endpoint_GetCurrentEndpoint()             (UENUM   &   ENDPOINT_EPNUM_MASK)
			#define Endpoint_SelectEndpoint(epnum)     MACROS{ UENUM    =  (epnum & ENDPOINT_EPNUM_MASK); }MACROE
			#define Endpoint_ResetFIFO(epnum)          MACROS{ UERST    =  (1 << (epnum & ENDPOINT_EPNUM_MASK)); UERST = 0; }MACROE
			#define Endpoint_EnableEndpoint()          MACROS{ UECONX  |=  (1 << EPEN);                   }MACROE
			#define Endpoint_DisableEndpoint()         MACROS{ UECONX  &= ~(1 << EPEN);                   }MACROE
			#define Endpoint_IsEnabled()                     ((UECONX  &   (1 << EPEN)) ? true : false)
			#define Endpoint_ResetEndpoint(epnum)      MACROS{ UENUM    =  (epnum & ENDPOINT_EPNUM_MASK); uint8_t temp = UECONX; UECONX = (temp | (1 << EPEN)); UECONX = (temp & ~(1 << EPEN)); }MACROE
			#define Endpoint_AllocateMemory()          MACROS{ UECFG1X |=  (1 << ALLOC);                  }MACROE
			#define Endpoint_DeallocateMemory()        MACROS{ UECFG1X &= ~(1 << ALLOC);                  }MACROE
			
			#define Endpoint_ReadWriteAllowed()              ((UEINTX  & (1 << RWAL)) ? true : false)

			
			#define Endpoint_ConfigureEndpoint(num, type, dir, size, banks)                           \
												       Endpoint_ConfigureEndpoint_P(num,              \
                                                       ((type << EPTYPE0) | dir),                     \
											           (Endpoint_BytesToEPSizeMask(size) | banks))
			#define Endpoint_IsConfigured()                  ((UESTA0X & (1 << CFGOK)) ? true : false)
			#define Endpoint_GetEndpointInterrupts()           UEINT
			#define Endpoint_ClearEndpointInterrupt(n) MACROS{ UEINT   &= ~(1 << n);                     }MACROE
			#define Endpoint_HasEndpointInterrupted(n)       ((UEINT   &   (1 << n)) ? true : false)

			#define Endpoint_FIFOCON_Clear()           MACROS{ UEINTX  &= ~(1 << FIFOCON);               }MACROE
			
			#define Endpoint_Setup_In_Clear()          MACROS{ UEINTX  &= ~(1 << TXINI);                 }MACROE
			#define Endpoint_Setup_Out_Clear()         MACROS{ UEINTX  &= ~(1 << RXOUTI);                }MACROE

			#define Endpoint_Setup_In_IsReady()              ((UEINTX  & (1 << TXINI))  ? true : false)
			#define Endpoint_Setup_Out_IsReceived()          ((UEINTX  & (1 << RXOUTI)) ? true : false)
			
			#define Endpoint_ClearSetupReceived()      MACROS{ UEINTX  &= ~(1 << RXSTPI);                }MACROE
			#define Endpoint_StallTransaction()        MACROS{ UECONX  |=  (1 << STALLRQ);               }MACROE
			#define Endpoint_ClearStall()              MACROS{ UECONX  |=  (1 << STALLRQC);              }MACROE

			#define Endpoint_IsStalled()                     ((UECONX  &   (1 << STALLRQ)) ? true : false)

			#define Endpoint_ResetDataToggle()         MACROS{ UECONX  |=  (1 << RSTDT);                 }MACROE

		/* Enums: */
			enum Endpoint_Stream_RW_ErrorCodes_t
			{
				ENDPOINT_RWSTREAM_ERROR_NoError            = 0,
				ENDPOINT_RWSTREAM_ERROR_EndpointStalled    = 1,				
				ENDPOINT_RWSTREAM_ERROR_DeviceDisconnected = 2,
			};

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
				uint8_t Dummy;
				
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
				uint8_t Dummy;
				
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
				uint8_t Dummy;
				
				Dummy = UEDATX;
				Dummy = UEDATX;
				Dummy = UEDATX;
				Dummy = UEDATX;
			}

		/* External Variables: */
			extern uint8_t USB_ControlEndpointSize;

		/* Function Prototypes: */
			void    Endpoint_ClearEndpoints(void);
			uint8_t Endpoint_Write_Stream_LE(void* Buffer, uint16_t Length) ATTR_NON_NULL_PTR_ARG(1);
			uint8_t Endpoint_Write_Stream_BE(void* Buffer, uint16_t Length) ATTR_NON_NULL_PTR_ARG(1);
			uint8_t Endpoint_Read_Stream_LE(void* Buffer, uint16_t Length)  ATTR_NON_NULL_PTR_ARG(1);
			uint8_t Endpoint_Read_Stream_BE(void* Buffer, uint16_t Length)  ATTR_NON_NULL_PTR_ARG(1);

		/* Function Aliases: */
			#define Endpoint_Read_Word()                   Endpoint_Read_Word_LE()   
			#define Endpoint_Write_Word(Word)              Endpoint_Write_Word_LE(Word)
			#define Endpoint_Read_DWord()                  Endpoint_Read_DWord_LE()
			#define Endpoint_Write_DWord(DWord)            Endpoint_Write_DWord_LE(DWord)
			#define Endpoint_Write_Stream(Data, Length)    Endpoint_Write_Stream_LE(Data, Length)
			#define Endpoint_Read_Stream(Buffer, Length)   Endpoint_Read_Stream_LE(Buffer, Length)

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define Endpoint_IsSetupRecieved()             ((UEINTX & (1 << RXSTPI)) ? true : false)
	
		/* Inline Functions: */
			static inline uint8_t Endpoint_BytesToEPSizeMask(const uint16_t Bytes)
			                                                 ATTR_WARN_UNUSED_RESULT ATTR_CONST;
			static inline uint8_t Endpoint_BytesToEPSizeMask(const uint16_t Bytes)
			{
				uint8_t SizeCheck = 8;
				uint8_t SizeMask  = 0;

				do
				{
					if (Bytes <= SizeCheck)
					  return (SizeMask << EPSIZE0);
					
					SizeCheck <<= 1;
					SizeMask++;
				} while (SizeCheck != (ENDPOINT_MAX_SIZE >> 1));
				
				return (SizeMask + 1);
			};

		/* Function Prototypes: */
			void Endpoint_ConfigureEndpoint_P(const uint8_t EndpointNum, const uint8_t UECFG0Xdata,
			                                  const uint8_t UECFG1Xdata);

#endif
