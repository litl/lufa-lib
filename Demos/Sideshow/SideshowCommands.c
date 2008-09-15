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

#define  INCLUDE_FROM_SIDESHOWCOMMANDS_H
#include "SideshowCommands.h"

UNICODE_STRING_t(80) UserSID  = {LengthInBytes: sizeof(SECURITY_INTERACTIVE_RID_SID),
                                 UnicodeString: SECURITY_INTERACTIVE_RID_SID};

Unicode_String_t DeviceName   = {LengthInBytes: sizeof(L"MyUSB Sideshow Device"),
                                 UnicodeString: L"MyUSB Sideshow Device"};

Unicode_String_t Manufacturer = {LengthInBytes: sizeof(L"Dean Camera"),
                                 UnicodeString: L"Dean Camera"};
								 
struct
{
	uint8_t TotalGadgets;
	GUID_t  GadgetGUIDs[4];
} CurrentlyAddedGadgets;

void Sideshow_ProcessCommandPacket(void)
{
	SideShow_PacketHeader_t PacketHeader;
	
	Endpoint_SelectEndpoint(SIDESHOW_OUT_EPNUM);	
	Endpoint_Read_Stream_LE(&PacketHeader, sizeof(SideShow_PacketHeader_t));
	
	PacketHeader.Type.Response = true;

	printf("\r\nCommand: %lX", PacketHeader.Type.TypeLong & 0x00FFFFFF);
	
	switch (PacketHeader.Type.TypeLong & 0x00FFFFFF)
	{
		case 0x01: // PING
			SideShow_Ping(&PacketHeader);
			break;
		case 0x502: // SYNC
			SideShow_Sync(&PacketHeader);
			break;
		case 0x101: // GET_CURRENT_USER
			SideShow_GetCurrentUser(&PacketHeader);
			break;
		case 0x100: // SET_CURRENT_USER
			SideShow_SetCurrentUser(&PacketHeader);
			break;
		case 0x500: // GET_DEVICE_NAME
			SideShow_GetString(&PacketHeader, &DeviceName);
			break;
		case 0x501: // GET_DEVICE_MANUFACTURER
			SideShow_GetString(&PacketHeader, &Manufacturer);
			break;
		case 0x104: // GET_APPLICATION_ORDER
			SideShow_GetApplicationOrder(&PacketHeader);
			break;
		case 0x117: // GET SUPPORTED ENDPOINTS
			SideShow_GetSupportedEndpoints(&PacketHeader);
			break;
		default:
			PacketHeader.Length -= sizeof(SideShow_PacketHeader_t);

			while (PacketHeader.Length--)
			{
				while (!(Endpoint_ReadWriteAllowed()));

				Endpoint_Discard_Byte();
				
				if (!(Endpoint_ReadWriteAllowed()))
				  Endpoint_ClearCurrentBank();
			}			

			PacketHeader.Length   = sizeof(SideShow_PacketHeader_t);
			PacketHeader.Type.NAK = true;
			
			Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);	
			Endpoint_Write_Stream_LE(&PacketHeader, sizeof(SideShow_PacketHeader_t));		
			Endpoint_ClearCurrentBank();

			printf(" (UNK)");
	}
}

static void SideShow_Read_Unicode_String(void* UnicodeStruct, uint16_t MaxBytes)
{
	uint32_t UnicodeCharsToRead = Endpoint_Read_DWord_LE();
	int16_t  UnicodeData[UnicodeCharsToRead];

	((Unicode_String_t*)UnicodeStruct)->LengthInBytes = (UnicodeCharsToRead << 1);

	Endpoint_Read_Stream_LE(&UnicodeData, ((Unicode_String_t*)UnicodeStruct)->LengthInBytes);
	
	if (((Unicode_String_t*)UnicodeStruct)->LengthInBytes > MaxBytes)
	  ((Unicode_String_t*)UnicodeStruct)->LengthInBytes = MaxBytes;
	  
	memcpy(&((Unicode_String_t*)UnicodeStruct)->UnicodeString, &UnicodeData,
	        ((Unicode_String_t*)UnicodeStruct)->LengthInBytes);
}

static void SideShow_Write_Unicode_String(void* UnicodeStruct)
{
	Endpoint_Write_DWord_LE(((Unicode_String_t*)UnicodeStruct)->LengthInBytes >> 1);

	Endpoint_Write_Stream_LE(&((Unicode_String_t*)UnicodeStruct)->UnicodeString,
	                         ((Unicode_String_t*)UnicodeStruct)->LengthInBytes);
}

static void SideShow_Ping(SideShow_PacketHeader_t* PacketHeader)
{
	Endpoint_ClearCurrentBank();

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);	
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));		
	Endpoint_ClearCurrentBank();
}

static void SideShow_Sync(SideShow_PacketHeader_t* PacketHeader)
{
	GUID_t ProtocolGUID;

	Endpoint_Read_Stream_LE(&ProtocolGUID, sizeof(ProtocolGUID));
	Endpoint_ClearCurrentBank();
	
	if (memcmp(&ProtocolGUID, (uint32_t[])STANDARD_PROTOCOL_GUID, sizeof(GUID_t)) != 0)
	  PacketHeader->Type.NAK = true;

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));		
	Endpoint_Write_Stream_LE(&ProtocolGUID, sizeof(GUID_t));
	Endpoint_ClearCurrentBank();
}

static void SideShow_GetCurrentUser(SideShow_PacketHeader_t* PacketHeader)
{
	Endpoint_ClearCurrentBank();

	PacketHeader->Length = sizeof(SideShow_PacketHeader_t) + sizeof(uint32_t) + UserSID.LengthInBytes;

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	SideShow_Write_Unicode_String(&UserSID);
	Endpoint_ClearCurrentBank();
}

static void SideShow_SetCurrentUser(SideShow_PacketHeader_t* PacketHeader)
{
	SideShow_Read_Unicode_String(&UserSID, sizeof(UserSID.UnicodeString));
	Endpoint_ClearCurrentBank();
	
	PacketHeader->Length = sizeof(SideShow_PacketHeader_t);

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_ClearCurrentBank();
}

static void SideShow_GetString(SideShow_PacketHeader_t* PacketHeader, void* UnicodeStruct)
{
	Endpoint_ClearCurrentBank();

	PacketHeader->Length = sizeof(SideShow_PacketHeader_t) +
	                       sizeof(uint32_t) + ((Unicode_String_t*)UnicodeStruct)->LengthInBytes;
	
	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	SideShow_Write_Unicode_String(UnicodeStruct);
	Endpoint_ClearCurrentBank();
}

static void SideShow_GetApplicationOrder(SideShow_PacketHeader_t* PacketHeader)
{
	Endpoint_ClearCurrentBank();

	uint16_t GadgetGUIDBytes = (CurrentlyAddedGadgets.TotalGadgets * sizeof(GUID_t));

	PacketHeader->Length = sizeof(SideShow_PacketHeader_t) +
	                       sizeof(uint32_t) + GadgetGUIDBytes;
	
	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_Write_DWord_LE(CurrentlyAddedGadgets.TotalGadgets);
	Endpoint_Write_Stream_LE(CurrentlyAddedGadgets.GadgetGUIDs, GadgetGUIDBytes);
	Endpoint_ClearCurrentBank();	
}

static void SideShow_GetSupportedEndpoints(SideShow_PacketHeader_t* PacketHeader)
{
	GUID_t SupportedEndpointGUID = (GUID_t){Chunks: SIMPLE_CONTENT_FORMAT_GUID};

	Endpoint_ClearCurrentBank();

	PacketHeader->Length = sizeof(SideShow_PacketHeader_t) + sizeof(uint32_t) + sizeof(GUID_t);
	
	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_Write_DWord_LE(1);
	Endpoint_Write_Stream_LE(&SupportedEndpointGUID, sizeof(GUID_t));
	Endpoint_ClearCurrentBank();
}
