/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/*
   ButtTag.H: This file can be included in user projects to
   allow the embedding of “ButtTags”, small strings of text
   which may be read by ButtLoad (if the program is stored in
   ButtLoad's non-volatile memory) or by other C management
   programs. Each ButtTag must have a unique name and be less
   that 20 characters in length.
   
   For more information about the ButtLoad project, visit:
     http://www.fourwalledcubicle.com/ButtLoad.php
*/

#ifndef __BUTTLOADTAG_H__
#define __BUTTLOADTAG_H__

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define BUTTLOADTAG(id, data) const struct ButtLoadTagData BUTTTAG_##id             \
			                               PROGMEM __attribute__((used, externally_visible)) =  \
			                               {MagicString: BT_TAGHEADER, TagData: data}
										  

	/* Private Interface - For use in library only: */	
		/* Macros: */
			#define BT_TAGHEADER          {'@','(','#',')'}

		/* Structures: */
			struct ButtLoadTagData
			{
				char MagicString[4];
				char TagData[];
			};

#endif
