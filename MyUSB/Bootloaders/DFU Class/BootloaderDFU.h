/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/boot.h>
		#include <avr/eeprom.h>
		#include <stdbool.h>
	
		#include "Descriptors.h"
		
		#include <MyUSB/Drivers/USB/USB.h>
		#include <MyUSB/Drivers/Board/LEDs.h>
		
	/* Macros: */
		// TEMP!!
		#define boot_signature_byte_get(x) 1
	
		#define BOOTLOADER_VERSION_MINOR 0
		#define BOOTLOADER_VERSION_REV   0
		#define BOOTLOADER_ID_BYTE1      0xDC
		#define BOOTLOADER_ID_BYTE2      0xFB

		#define BOOTLOADER_VERSION       ((BOOTLOADER_VERSION_MINOR << 4) | BOOTLOADER_VERSION_REV)
		
		#define IS_ONEBYTE_COMMAND(dataarr, cb1)       (dataarr[0] == cb1)
		#define IS_TWOBYTE_COMMAND(dataarr, cb1, cb2) ((dataarr[0] == cb1) && (dataarr[1] == cb2))
	
		#define DFU_FILE_SUFFIX_SIZE     16
		#define DFU_FILLER_BYTES_SIZE    26
	
		#define DFU_DETATCH              0x00
		#define DFU_DNLOAD               0x01
		#define DFU_UPLOAD               0x02
		#define DFU_GETSTATUS            0x03
		#define DFU_CLRSTATUS            0x04
		#define DFU_GETSTATE             0x05
		#define DFU_ABORT                0x06

		#define COMMAND_PROG_START       0x01
		#define COMMAND_DISP_DATA        0x03
		#define COMMAND_WRITE            0x04
		#define COMMAND_READ             0x05
		#define COMMAND_CHANGE_BASE_ADDR 0x06

	/* Type Defines: */
		typedef void (*AppPtr_t)(void) ATTR_NO_RETURN;
		
		typedef struct
		{
			uint8_t  Command;
			uint8_t  Data[5];
			uint16_t DataSize;
		} DFU_Command_t;

	/* Enums: */
		enum DFU_State_t
		{
			appIDLE                      = 0,
			appDETACH                    = 1,
			dfuIDLE                      = 2,
			dfuDNLOAD_SYNC               = 3,
			dfuDNBUSY                    = 4,
			dfuDNLOAD_IDLE               = 5,
			dfuMANIFEST_SYNC             = 6,
			dfuMANIFEST                  = 7,
			dfuMANIFEST_WAIT_RESET       = 8,
			dfuUPLOAD_IDLE               = 9,
			dfuERROR	                 = 10
		};

		enum DFU_Status_t
		{
			OK                           = 0,
			errTARGET                    = 1,
			errFILE                      = 2,
			errWRITE                     = 3,
			errERASE                     = 4,
			errCHECK_ERASED              = 5,
			errPROG                      = 6,
			errVERIFY                    = 7,
			errADDRESS                   = 8,
			errNOTDONE                   = 9,
			errFIRMWARE                  = 10,
			errVENDOR                    = 11,
			errUSBR                      = 12,
			errPOR                       = 13,
			errUNKNOWN                   = 14,
			errSTALLEDPKT	             = 15
		};

	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		
	/* Function Prototypes: */
		#if defined(INCLUDE_FROM_BOOTLOADER_C)
			static void DiscardFillerBytes(uint8_t FillerBytes);
			static void ProcessBootloaderCommand(void);
			static void LoadStartEndAddresses(void);
			static void ProcessMemProgCommand(void);
			static void ProcessMemReadCommand(void);
			static void ProcessWriteCommand(void);
			static void ProcessReadCommand(void);
		#endif
		
#endif
