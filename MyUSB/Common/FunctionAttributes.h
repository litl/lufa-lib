/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __FUNCATTR_H__
#define __FUNCATTR_H__

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define ATTR_NO_RETURN            __attribute__ ((noreturn))
			#define ATTR_INIT_SECTION(x)      __attribute__ ((naked, section (".init" #x )))
			#define ATTR_WARN_UNUSED_RESULT   __attribute__ ((warn_unused_result))
			#define ATTR_NON_NULL_PTR_ARG(x)  __attribute__ ((nonnull (x)))
			#define ATTR_NAKED                __attribute__ ((naked))
			#define ATTR_PURE                 __attribute__ ((pure))
			#define ATTR_CONST                __attribute__ ((const))
			#define ATTR_DEPRECATED           __attribute__ ((deprecated))
			#define ATTR_WEAK                 __attribute__ ((weak))
			#define ATTR_ALIAS(x)             __attribute__ ((alias( #x )))
		
#endif
