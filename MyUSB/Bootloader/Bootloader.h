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
		#include "Descriptors.h"
		#include "BootFuncs.h"
		
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/boot.h>
		#include <avr/eeprom.h>
		#include <stdbool.h>
	
		#include <MyUSB/Drivers/USB/USB.h>
		#include <MyUSB/Drivers/USBKEY/Bicolour.h>
		
	/* Macros: */
		#define BOOTLOADER_VERSION_MINOR 0
		#define BOOTLOADER_VERSION_REV   0

		#define BOOTLOADER_VERSION       ((BOOTLOADER_VERSION_MINOR << 4) | BOOTLOADER_VERSION_REV)
		
		#define IS_ONEBYTE_COMMAND(dataarray, cb1)       (dataarray[0] == cb1)
		#define IS_TWOBYTE_COMMAND(dataarray, cb1, cb2) ((dataarray[0] == cb1) && (dataarray[1] == cb2))
	
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
		
		#define OP_SINGLE_BYTE_RESPONSE  0x00
		#define OP_READ_FLASH            0x01
		#define OP_WRITE_FLASH           0x02
		#define OP_BLANK_CHECK           0x03
		#define OP_READ_EEPROM           0x04
		#define OP_WRITE_EEPROM          0x05

	/* Type Defines: */
		typedef void (*FuncPtr_t)(void);
		
		typedef struct
		{
			uint8_t Command;
			uint8_t Data[5];
			uint16_t DataSize;
		} DFU_Command_t;

	/* Enums: */
		enum
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
		} DFU_State_t;

		enum
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
		} DFU_Status_t;

	/* Event Handlers: */
		HANDLES_EVENT(USB_Connect);
		HANDLES_EVENT(USB_Disconnect);
		HANDLES_EVENT(USB_UnhandledControlPacket);
		
	/* Function Prototypes: */
		#if defined(INCLUDE_FROM_BOOTLOADER_C)
			static void ProcessBootloaderCommand(void);
			static void ProcessMemProgCommand(void);
			static void ProcessMemReadCommand(void);
			static void ProcessWriteCommand(void);
			static void ProcessReadCommand(void);
			static void EraseFlash(void);
		#endif
		
#endif
