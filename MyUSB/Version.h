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
 
/** \mainpage MyUSB Library
 *
 *  Written by Dean Camera, 2008.
 *
 *  \section Sec_About About this library
 *
 *  Originally based on the AT90USBKEY from Atmel, it is an open-source, driver for the USB-enabled AVR
 *  microcontrollers, released under the Lesser GNU Public Licence, Version 3. Currently, the AT90USB1286,
 *  AT90USB1287, AT90USB646, AT90USB647, AT90USB162, AT90USB82 and ATMEGA32U4 AVR microcontrollers are supported
 *  by the library. Supported boards are the USBKEY, STK525, STK526 and RZUSBSTICK.
 *
 *  The library is currently in a stable release, suitable for download and incorporation into user projects for
 *  both host and device modes. For information about the project progression, check out my blog.
 *
 *  MyUSB is written specifically for the free AVR-GCC compiler, and uses several GCC-only extensions to make the
 *  library API more streamlined and robust. You can download AVR-GCC for free in a convenient windows package, 
 *  from the the WinAVR website.
 *
 *  \section Sec_Links Library Links
 *  Project Homepage: http://www.fourwalledcubicle.com/MyUSB.php \n
 *  Development Blog: http://www.fourwalledcubicle.com/blog \n
 *  Author's Website: http://www.fourwalledcubicle.com \n
 *
 *  WinAVR Website: http://winavr.sourceforge.net \n
 *  avr-libc Website: http://www.nongnu.org/avr-libc/ \n
 *
 *  USB-IF Website: http://www.usb.org \n
 *
 *  \section Sec_License License
 *  The MyUSB library is currently released under the LGPL licence, version 3. Please see the license file
 *  included with the library before use.
 *
 *  \section Sec_Demos Demos and Bootloaders
 *  The MyUSB library ships with several different host and device demos, located in the /Demos/ subdirectory.
 *  If this directory is missing, please re-download the project from the project homepage.
 *
 *  Also included with the library are two fully functional bootloaders, one using the DFU USB class and is
 *  compatible with Atmel's FLIP software or the open source dfu-programmer project, and the other using the
 *  CDC class and the AVR109 protocol to be compatible with such open source software as avrdude and AVR-OSP.
 *
 *  \section Sec_Donations Donate
 *  Please consider donating a small amount to myself, to support this and my future Open Source projects. You
 *  can donate any amount via PayPal on my website, http://www.fourwalledcubicle.com . All donations are greatly
 *  appreciated.
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
