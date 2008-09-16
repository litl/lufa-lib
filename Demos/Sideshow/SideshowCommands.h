/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _SIDESHOW_COMMANDS_H_
#define _SIDESHOW_COMMANDS_H_

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		#include <string.h>
	
		#include "Sideshow.h"
		#include "SideshowCommon.h"
		#include "SideshowApplications.h"
		#include "SideshowContent.h"

	/* Macros: */
		#define SIDESHOW_CMD_PING                     0x001
		#define SIDESHOW_CMD_SET_CURRENT_USER         0x100
		#define SIDESHOW_CMD_GET_CURRENT_USER         0x101
		#define SIDESHOW_CMD_GET_APPLICATION_ORDER    0x104
		#define SIDESHOW_CMD_ADD_APPLICATION          0x10D
		#define SIDESHOW_CMD_DELETE_APPLICATION       0x10E
		#define SIDESHOW_CMD_DELETE_ALL_APPLICATIONS  0x10F
		#define SIDESHOW_CMD_ADD_CONTENT              0x114
		#define SIDESHOW_CMD_DELETE_CONTENT           0x115
		#define SIDESHOW_CMD_DELETE_ALL_CONTENT       0x116
		#define SIDESHOW_CMD_GET_SUPPORTED_ENDPOINTS  0x117
		#define SIDESHOW_CMD_GET_DEVICE_NAME          0x500
		#define SIDESHOW_CMD_GET_MANUFACTURER         0x501
		#define SIDESHOW_CMD_SYNC                     0x502
		
	/* Type Defines: */	
		typedef union
		{
			uint32_t TypeLong;

			struct
			{
				uint8_t TypeBytes[3];

				int ErrorCode     : 6;
				int NAK           : 1;
				int Response      : 1;				
			};
		} SideShowPacketType_t;
	
		typedef struct
		{
			uint32_t               Length;
			SideShowPacketType_t   Type;
			uint16_t               Number;
		} SideShow_PacketHeader_t;
				
	/* Function Prototypes: */
		void Sideshow_ProcessCommandPacket(void);
		
		#if defined(INCLUDE_FROM_SIDESHOWCOMMANDS_H)
			static void SideShow_Ping(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_Sync(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_GetCurrentUser(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_SetCurrentUser(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_GetString(SideShow_PacketHeader_t* PacketHeader, void* UnicodeStruct);
			static void SideShow_GetApplicationOrder(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_GetSupportedEndpoints(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_AddApplication(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_DeleteApplication(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_DeleteAllApplications(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_AddContent(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_DeleteContent(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_DeleteAllContent(SideShow_PacketHeader_t* PacketHeader);
		#endif

#endif
