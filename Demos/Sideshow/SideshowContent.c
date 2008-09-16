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


bool SideShow_AddSimpleContent(GUID_t* ApplicationID)
{
	SideShow_Content_t* Content = SideShow_GetFreeContent();
	uint32_t            ContentSize;
	uint32_t            ContentID;
	
	Endpoint_Read_Stream_LE(&ContentID, sizeof(uint32_t));

	if (Content == NULL)
	{
		SideShow_Discard_Byte_Stream();
		return false;
	}
	
	Endpoint_Read_Stream_LE(&ContentSize, sizeof(uint32_t));

	printf("Size: %lu\r\n", ContentSize);
	
	while (ContentSize--)
	{
		if (!(Endpoint_ReadWriteAllowed()))
		{
			Endpoint_ClearCurrentBank();

			while (!(Endpoint_ReadWriteAllowed()));
		}

		printf("%c", Endpoint_Read_Byte());
	}
	
	// TODO: Create content ID object, read in content data
	//       Associate application to content

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
