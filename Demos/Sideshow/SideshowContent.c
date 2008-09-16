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

#define  INCLUDE_FROM_SIDESHOWCONTENT_C
#include "SideshowContent.h"

SideShow_Content_t Content[MAX_CONTENT];


bool SideShow_AddSimpleContent(SideShow_PacketHeader_t* PacketHeader, GUID_t* ApplicationID)
{
	SideShow_Content_t* AppContent;
	uint32_t            ContentSize;
	uint32_t            ContentID;
		
	Endpoint_Read_Stream_LE(&ContentID, sizeof(uint32_t));

	AppContent = SideShow_GetContentByID(ApplicationID, ContentID);

	if (AppContent == NULL)
	  AppContent = SideShow_GetFreeContent();
	else
	  printf(" <Updated>");
	
	if (AppContent == NULL)
	{
		PacketHeader->Length -= sizeof(SideShow_PacketHeader_t) + (2 * sizeof(GUID_t)) + sizeof(uint32_t);
		
		printf(" CONTENT FULL");

		Endpoint_Discard_Stream(PacketHeader->Length);
		return false;
	}
	
	Endpoint_Read_Stream_LE(&ContentSize, sizeof(uint32_t));
	Endpoint_Read_Stream_LE(&AppContent->ContentData, sizeof("<body>") - 1);
	
	if (!(memcmp(&AppContent->ContentData, "<body>", (sizeof("<body>") - 1))))
	{
		printf(" XML");

		AppContent->ApplicationID = *ApplicationID;
		AppContent->ContentID     = ContentID;
		AppContent->InUse         = true;

		// PROCESS	

		ContentSize -= (sizeof("<body>") - 1);
		Endpoint_Discard_Stream(ContentSize);	
	}
	else
	{
		printf(" BINARY");

		ContentSize -= (sizeof("<body>") - 1);
		Endpoint_Discard_Stream(ContentSize);
	}
	
	return true;
}

static SideShow_Content_t* SideShow_GetFreeContent(void)
{
	for (int ContentItem = 0; ContentItem < MAX_CONTENT; ContentItem++)
	{
		if (!(Content[ContentItem].InUse))
		  return &Content[ContentItem];
	}
	
	return NULL;
}

static SideShow_Content_t* SideShow_GetContentByID(GUID_t* ApplicationID, uint32_t ContentID)
{
	for (int ContentItem = 0; ContentItem < MAX_CONTENT; ContentItem++)
	{
		if (Content[ContentItem].InUse &&
		   (Content[ContentItem].ContentID == ContentID) &&
		   (!(memcmp(&Content[ContentItem].ApplicationID, ApplicationID, sizeof(GUID_t)))))
		{
			return &Content[ContentItem];
		}
	}
	
	return NULL;
}
