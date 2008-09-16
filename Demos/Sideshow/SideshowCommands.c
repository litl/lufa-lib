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


void Sideshow_ProcessCommandPacket(void)
{
	SideShow_PacketHeader_t PacketHeader;
	
	Endpoint_SelectEndpoint(SIDESHOW_OUT_EPNUM);	
	Endpoint_Read_Stream_LE(&PacketHeader, sizeof(SideShow_PacketHeader_t));
	
	PacketHeader.Type.Response = true;

	printf("\r\nCommand: %lX", PacketHeader.Type.TypeLong & 0x00FFFFFF);
	
	switch (PacketHeader.Type.TypeLong & 0x00FFFFFF)
	{
		case SIDESHOW_CMD_PING:
			SideShow_Ping(&PacketHeader);
			break;
		case SIDESHOW_CMD_SYNC:
			SideShow_Sync(&PacketHeader);
			break;
		case SIDESHOW_CMD_GET_CURRENT_USER:
			SideShow_GetCurrentUser(&PacketHeader);
			break;
		case SIDESHOW_CMD_SET_CURRENT_USER:
			SideShow_SetCurrentUser(&PacketHeader);
			break;
		case SIDESHOW_CMD_GET_DEVICE_NAME:
			SideShow_GetString(&PacketHeader, &DeviceName);
			break;
		case SIDESHOW_CMD_GET_MANUFACTURER:
			SideShow_GetString(&PacketHeader, &Manufacturer);
			break;
		case SIDESHOW_CMD_GET_APPLICATION_ORDER:
			SideShow_GetApplicationOrder(&PacketHeader);
			break;
		case SIDESHOW_CMD_GET_SUPPORTED_ENDPOINTS:
			SideShow_GetSupportedEndpoints(&PacketHeader);
			break;
		case SIDESHOW_CMD_ADD_APPLICATION:
			SideShow_AddApplication(&PacketHeader);
			break;
		case SIDESHOW_CMD_ADD_CONTENT:
			SideShow_AddContent(&PacketHeader);
			break;
		case SIDESHOW_CMD_DELETE_CONTENT:
			SideShow_DeleteContent(&PacketHeader);
			break;
		case SIDESHOW_CMD_DELETE_ALL_CONTENT:
			SideShow_DeleteAllContent(&PacketHeader);
			break;		
		case SIDESHOW_CMD_DELETE_APPLICATION:
			SideShow_DeleteApplication(&PacketHeader);
			break;
		case SIDESHOW_CMD_DELETE_ALL_APPLICATIONS:
			SideShow_DeleteAllApplications(&PacketHeader);
			break;
		default:
			PacketHeader.Length -= sizeof(SideShow_PacketHeader_t);

			Endpoint_Discard_Stream(PacketHeader.Length);
			Endpoint_ClearCurrentBank();

			PacketHeader.Length   = sizeof(SideShow_PacketHeader_t);
			PacketHeader.Type.NAK = true;
			
			Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);	
			Endpoint_Write_Stream_LE(&PacketHeader, sizeof(SideShow_PacketHeader_t));		
			Endpoint_ClearCurrentBank();

			printf(" (UNK)");
	}
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

	Endpoint_Read_Stream_LE(&ProtocolGUID, sizeof(GUID_t));
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
	uint8_t TotalInstalledApplications = SideShow_GetTotalApplications();
	uint16_t GadgetGUIDBytes           = (TotalInstalledApplications * sizeof(GUID_t));

	Endpoint_ClearCurrentBank();

	PacketHeader->Length = sizeof(SideShow_PacketHeader_t) +
	                       sizeof(uint32_t) + GadgetGUIDBytes;
	
	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_Write_DWord_LE(TotalInstalledApplications);
	
	for (uint8_t App = 0; App < MAX_APPLICATIONS; App++)
	{
		if (InstalledApplications[App].InUse == true)
		  Endpoint_Write_Stream_LE(&InstalledApplications[App].ApplicationID, sizeof(GUID_t));
	}

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

static void SideShow_AddApplication(SideShow_PacketHeader_t* PacketHeader)
{
	SideShow_Application_t* CurrApp = SideShow_GetFreeApplication();

	PacketHeader->Length -= sizeof(SideShow_PacketHeader_t);

	if (CurrApp != NULL)
	{
		Endpoint_Read_Stream_LE(&CurrApp->ApplicationID, sizeof(GUID_t));
		Endpoint_Read_Stream_LE(&CurrApp->EndpointID, sizeof(GUID_t));
		SideShow_Read_Unicode_String(&CurrApp->ApplicationName, sizeof(CurrApp->ApplicationName.UnicodeString));
		Endpoint_Read_Stream_LE(&CurrApp->CachePolicy, sizeof(uint32_t));
		Endpoint_Read_Stream_LE(&CurrApp->OnlineOnly, sizeof(uint32_t));
		SideShow_Discard_Byte_Stream();
		SideShow_Discard_Byte_Stream();
		SideShow_Discard_Byte_Stream();
		Endpoint_ClearCurrentBank();
		
		CurrApp->InUse = true;
	}
	else
	{
		Endpoint_Discard_Stream(PacketHeader->Length);

		PacketHeader->Type.NAK = true;
	}

	PacketHeader->Length = sizeof(SideShow_PacketHeader_t);

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_ClearCurrentBank();
}

static void SideShow_DeleteApplication(SideShow_PacketHeader_t* PacketHeader)
{
	GUID_t ApplicationGUID;
	SideShow_Application_t* AppToDelete;
	
	Endpoint_Read_Stream_LE(&ApplicationGUID, sizeof(GUID_t));	
	Endpoint_ClearCurrentBank();

	AppToDelete = SideShow_GetApplicationFromGUID(&ApplicationGUID);

	if (AppToDelete != NULL)
	  AppToDelete->InUse = false;
	else
	  PacketHeader->Type.NAK = true;

	PacketHeader->Length = sizeof(SideShow_PacketHeader_t);

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_ClearCurrentBank();
}

static void SideShow_DeleteAllApplications(SideShow_PacketHeader_t* PacketHeader)
{
	Endpoint_ClearCurrentBank();
	
	for (uint8_t App = 0; App < MAX_APPLICATIONS; App++)
	  InstalledApplications[App].InUse = false;

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_ClearCurrentBank();	
}

static void SideShow_AddContent(SideShow_PacketHeader_t* PacketHeader)
{
	GUID_t ApplicationID;
	GUID_t EndpointID;
	SideShow_Application_t* Application;
	
	Endpoint_Read_Stream_LE(&ApplicationID, sizeof(GUID_t));
	Endpoint_Read_Stream_LE(&EndpointID, sizeof(GUID_t));
	
	Application = SideShow_GetApplicationFromGUID(&ApplicationID);
	
	if (Application == NULL)
	{
		SideShow_Discard_Byte_Stream();
		PacketHeader->Type.NAK = true;
	}
	else if (!(SideShow_AddSimpleContent(&ApplicationID)))
	{
		PacketHeader->Type.NAK = true;
	}
	
	Endpoint_ClearCurrentBank();

	PacketHeader->Length = sizeof(SideShow_PacketHeader_t);

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_ClearCurrentBank();
}

static void SideShow_DeleteContent(SideShow_PacketHeader_t* PacketHeader)
{
	GUID_t   ApplicationID;
	GUID_t   EndpointID;
	uint32_t ContentID;
	bool     ContentFound = false;

	Endpoint_Read_Stream_LE(&ApplicationID, sizeof(GUID_t));
	Endpoint_Read_Stream_LE(&EndpointID, sizeof(GUID_t));
	Endpoint_Read_Stream_LE(&ContentID, sizeof(uint32_t));
	Endpoint_ClearCurrentBank();
	
	for (int ContentItem = 0; ContentItem < MAX_CONTENT; ContentItem++)
	{
		if ((Content[ContentItem].ContentID == ContentID) &&
		     memcmp(&Content[ContentItem].ApplicationID, &ApplicationID, sizeof(GUID_t)))
		{
			Content->InUse = false;
			ContentFound = true;
		}
	}
	
	if (!(ContentFound))
	  PacketHeader->Type.NAK = true;
	  
	PacketHeader->Length = sizeof(SideShow_PacketHeader_t);

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_ClearCurrentBank();	  
}

static void SideShow_DeleteAllContent(SideShow_PacketHeader_t* PacketHeader)
{
	GUID_t ApplicationID;
	GUID_t EndpointID;

	Endpoint_Read_Stream_LE(&ApplicationID, sizeof(GUID_t));
	Endpoint_Read_Stream_LE(&EndpointID, sizeof(GUID_t));
	Endpoint_ClearCurrentBank();

	for (int ContentItem = 0; ContentItem < MAX_CONTENT; ContentItem++)
	{
		if (memcmp(&Content[ContentItem].ApplicationID, &ApplicationID, sizeof(GUID_t)))
		  Content[ContentItem].InUse = false;
	}  

	PacketHeader->Length = sizeof(SideShow_PacketHeader_t);

	Endpoint_SelectEndpoint(SIDESHOW_IN_EPNUM);
	Endpoint_Write_Stream_LE(PacketHeader, sizeof(SideShow_PacketHeader_t));
	Endpoint_ClearCurrentBank();	  
}
