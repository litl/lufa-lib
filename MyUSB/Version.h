/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

/** \file
 *
 *  Version constants for informational purposes and version-specific macro creation. This header file contains the
 *  current MyUSB version number in several forms, for use in the user-application (for example, for printing out 
 *  whilst debugging, or for testing for version compatibility).
 */

#ifndef __MYUSB_VERSION_H__
#define __MYUSB_VERSION_H__

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Indicates the major version number of the library as an integer. */
			#define MYUSB_VERSION_MAJOR    1

			/** Indicates the minor version number of the library as an integer. */
			#define MYUSB_VERSION_MINOR    5

			/** Indicates the revision version number of the library as an integer. */
			#define MYUSB_VERSION_REVISION 0

			/** Indicates the complete version number of the library, in float form. */
			#define MYUSB_VERSION          1.5.0f

			/** Indicates the complete version number of the library, in string form. */
			#define MYUSB_VERSION_STRING   "1.5.0"

#endif
