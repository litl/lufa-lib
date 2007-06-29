#ifndef FUNCATTR_H
#define FUNCATTR_H

	/* Public Macros */
		#define ATTR_NO_RETURN            __attribute__ ((noreturn))
		#define ATTR_INIT_SECTION(x)      __attribute__ ((naked, section (".init" #x )))
		#define ATTR_WARN_UNUSED_RESULT   __attribute__ ((warn_unused_result))
		#define ATTR_NON_NULL_PTR_ARGS(x) __attribute__ ((nonnull (x)))
		#define ATTR_NAKED                __attribute__ ((naked))
	
#endif
