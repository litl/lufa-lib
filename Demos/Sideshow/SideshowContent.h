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

#ifndef _SIDESHOW_CONTENT_H_
#define _SIDESHOW_CONTENT_H_

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>
		#include <string.h>

		#include "SideshowCommon.h"

	/* Macros: */
		#define MAX_CONTENT    5
	
	/* Type Defines: */
		typedef struct
		{
			bool               InUse;
			GUID_t             ApplicationID;
			uint32_t           ContentID;
		} SideShow_Content_t;
		
	/* External Variables: */
		extern SideShow_Content_t Content[MAX_CONTENT];

	/* Function Prototypes: */
		bool SideShow_AddSimpleContent(GUID_t* ApplicationID);
		
		#if defined(INCLUDE_FROM_SIDESHOWCONTENT_C)
			static SideShow_Content_t* SideShow_GetFreeContent(void);
		#endif

#endif