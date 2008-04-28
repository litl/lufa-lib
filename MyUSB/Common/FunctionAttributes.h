/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#if !defined(__COMMON_H__)
	#error Do not include this file directly. Include MyUSB/Common/Common.h instead to gain this functionality.
#endif

#if !defined(__FUNCATTR_H__) && defined(__COMMON_H__)
#define __FUNCATTR_H__

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define ATTR_NO_RETURN              __attribute__ ((noreturn))
			#define ATTR_INIT_SECTION(x)        __attribute__ ((naked, section (".init" #x )))
			#define ATTR_WARN_UNUSED_RESULT     __attribute__ ((warn_unused_result))
			#define ATTR_NON_NULL_PTR_ARG(...)  __attribute__ ((nonnull (__VA_ARGS__)))
			#define ATTR_NAKED                  __attribute__ ((naked))
			#define ATTR_NOINLINE               __attribute__ ((noinline))
			#define ATTR_PURE                   __attribute__ ((pure))
			#define ATTR_CONST                  __attribute__ ((const))
			#define ATTR_DEPRECATED             __attribute__ ((deprecated))
			#define ATTR_WEAK                   __attribute__ ((weak))
			#define ATTR_ALIAS(x)               __attribute__ ((alias( #x )))
		
#endif
