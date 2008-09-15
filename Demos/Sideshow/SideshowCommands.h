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
		#include <stdbool.h>
		#include <string.h>
	
		#include "Sideshow.h"
	
		#include <MyUSB/Drivers/USB/USB.h>            // USB Functionality

	/* Type Defines: */
		typedef struct
		{
			uint16_t LengthInBytes;
			int      UnicodeString[];
		} Unicode_String_t;
		
		typedef struct
		{
			uint32_t Chunks[4];
		} GUID_t;		
	
		typedef union
		{
			uint32_t TypeLong;

			struct
			{
				uint8_t TypeBytes[3];

				int ErrorCode : 6;
				int NAK       : 1;
				int Response  : 1;				
			};
		} SideShowPacketType_t;
	
		typedef struct
		{
			uint32_t             Length;
			SideShowPacketType_t Type;
			uint16_t             Number;
		} SideShow_PacketHeader_t;
		
	/* Macros: */
		#define STANDARD_PROTOCOL_GUID        {0xA33F248B, 0x4531882F, 0x3BEDC282, 0x20C5C590}
		#define SIMPLE_CONTENT_FORMAT_GUID    {0xA9A5353F, 0x47CE2D4B, 0x9F75EE93, 0x4FDA7D3A}
		
		#define SECURITY_INTERACTIVE_RID_SID  L"S-1-5-4"

		#define UNICODE_STRING_t(x)		      struct                          \
		                                      {                               \
		                                          uint16_t LengthInBytes;    \
		                                          int      UnicodeString[x]; \
		                                      }

	/* Function Prototypes: */
		void Sideshow_ProcessCommandPacket(void);
		
		#if defined(INCLUDE_FROM_SIDESHOWCOMMANDS_H)
			static void SideShow_Read_Unicode_String(void* UnicodeStruct, uint16_t MaxBytes);
			static void SideShow_Write_Unicode_String(void* UnicodeStruct);
			static void SideShow_Ping(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_Sync(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_GetCurrentUser(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_SetCurrentUser(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_GetString(SideShow_PacketHeader_t* PacketHeader, void* UnicodeStruct);
			static void SideShow_GetApplicationOrder(SideShow_PacketHeader_t* PacketHeader);
			static void SideShow_GetSupportedEndpoints(SideShow_PacketHeader_t* PacketHeader);
		#endif

#endif
