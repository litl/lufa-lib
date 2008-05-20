/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Board specific Dataflash commands header for the AT45DB642D as mounted on the USBKEY.
 *
 *  \note This file should not be included directly. It is automatically included as needed by the dataflash driver
 *        dispatch header located in MyUSB/Drivers/Board/Dataflash.h.
 */

#ifndef __DATAFLASH_CMDS_H__
#define __DATAFLASH_CMDS_H__

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define DF_STATUS_READY                         (1 << 7)
			#define DF_STATUS_COMPMISMATCH                  (1 << 6)
			#define DF_STATUS_SECTORPROTECTION_ON           (1 << 1)
			#define DF_STATUS_BINARYPAGESIZE_ON             (1 << 0)
			
			#define DF_MANUFACTURER_ATMEL                   0x1F
		
			#define DF_CMD_GETSTATUS                        0xD7
			#define DF_CMD_POWERDOWN                        0xB9
			#define DF_CMD_WAKEUP                           0xAB

			#define DF_CMD_MAINMEMTOBUFF1                   0x53
			#define DF_CMD_MAINMEMTOBUFF2                   0x55
			#define DF_CMD_MAINMEMTOBUFF1COMP               0x60
			#define DF_CMD_MAINMEMTOBUFF2COMP               0x61
			#define DF_CMD_AUTOREWRITEBUFF1                 0x58
			#define DF_CMD_AUTOREWRITEBUFF2                 0x59
			
			#define DF_CMD_MAINMEMPAGEREAD                  0xD2
			#define DF_CMD_CONTARRAYREAD_LF                 0x03
			#define DF_CMD_BUFF1READ_LF                     0xD1
			#define DF_CMD_BUFF2READ_LF                     0xD3
			
			#define DF_CMD_BUFF1WRITE                       0x84
			#define DF_CMD_BUFF2WRITE                       0x87
			#define DF_CMD_BUFF1TOMAINMEMWITHERASE          0x83
			#define DF_CMD_BUFF2TOMAINMEMWITHERASE          0x86
			#define DF_CMD_BUFF1TOMAINMEM                   0x88
			#define DF_CMD_BUFF2TOMAINMEM                   0x89
			#define DF_CMD_MAINMEMPAGETHROUGHBUFF1          0x82
			#define DF_CMD_MAINMEMPAGETHROUGHBUFF2          0x85
			
			#define DF_CMD_PAGEERASE                        0x81
			#define DF_CMD_BLOCKERASE                       0x50
			#define DF_CMD_SECTORERASE                      0x7C

			#define DF_CMD_CHIPERASE_BYTE1                  0xC7
			#define DF_CMD_CHIPERASE_BYTE2                  0x94
			#define DF_CMD_CHIPERASE_BYTE3                  0x80
			#define DF_CMD_CHIPERASE_BYTE4                  0x9A
			
			#define DF_CMD_SECTORPROTECTIONOFF_BYTE1        0x3D
			#define DF_CMD_SECTORPROTECTIONOFF_BYTE2        0x2A
			#define DF_CMD_SECTORPROTECTIONOFF_BYTE3        0x7F
			#define DF_CMD_SECTORPROTECTIONOFF_BYTE4        0x9A
			
			#define DF_CMD_READMANUFACTURERDEVICEINFO       0x9F

#endif
