/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the GPL Licence, Version 3
*/

#ifndef __TERMINALCODES_H__
#define __TERMINALCODES_H__

	/* Public Interface - May be used in end-application: */
		/* Defines: */
			#define ESCAPE_CODE(c)           "\33[" c

			#define ESC_RESET                ESCAPE_CODE("0m")
			#define ESC_BOLD_ON              ESCAPE_CODE("1m")
			#define ESC_ITALIC_ON            ESCAPE_CODE("3m")
			#define ESC_UNDERLINE_ON         ESCAPE_CODE("4m")
			#define ESC_INVERSE_ON           ESCAPE_CODE("7m")
			#define ESC_STRIKETHROUGH_ON     ESCAPE_CODE("9m")
			#define ESC_BOLD_OFF             ESCAPE_CODE("22m")
			#define ESC_ITALICS_OFF          ESCAPE_CODE("23m")
			#define ESC_UNDERLINE_OFF        ESCAPE_CODE("24m")
			#define ESC_INVERSE_OFF          ESCAPE_CODE("27m")
			#define ESC_STRIKETHROUGH_OFF    ESCAPE_CODE("29m")
			#define ESC_FG_BLACK             ESCAPE_CODE("30m")
			#define ESC_FG_RED               ESCAPE_CODE("31m")
			#define ESC_FG_GREEN             ESCAPE_CODE("32m")
			#define ESC_FG_YELLOW            ESCAPE_CODE("33m")
			#define ESC_FG_BLUE              ESCAPE_CODE("34m")
			#define ESC_FG_MAGENTA           ESCAPE_CODE("35m")
			#define ESC_FG_CYAN              ESCAPE_CODE("36m")
			#define ESC_FG_WHITE             ESCAPE_CODE("37m")
			#define ESC_FG_DEFAULT           ESCAPE_CODE("39m")
			#define ESC_BG_BLACK             ESCAPE_CODE("40m")
			#define ESC_BG_RED               ESCAPE_CODE("41m")
			#define ESC_BG_GREEN             ESCAPE_CODE("42m")
			#define ESC_BG_YELLOW            ESCAPE_CODE("43m")
			#define ESC_BG_BLUE              ESCAPE_CODE("44m")
			#define ESC_BG_MAGENTA           ESCAPE_CODE("45m")
			#define ESC_BG_CYAN              ESCAPE_CODE("46m")
			#define ESC_BG_WHITE             ESCAPE_CODE("47m")
			#define ESC_BG_DEFAULT           ESCAPE_CODE("49m")
			
			#define ESC_CURSOR_POS(L, C)     ESCAPE_CODE("P" #L ";P" #C "H")
			#define ESC_CURSOR_UP(L)         ESCAPE_CODE("P" #L "A")
			#define ESC_CURSOR_DOWN(L)       ESCAPE_CODE("P" #L "B")
			#define ESC_CURSOR_FORWARD(C)    ESCAPE_CODE("P" #C "C")
			#define ESC_CURSOR_BACKWARD(C)   ESCAPE_CODE("P" #C "D")
			#define ESC_CURSOR_POS_SAVE      ESCAPE_CODE("s")
			#define ESC_CURSOR_POS_RESTORE   ESCAPE_CODE("u")
			
			#define ESC_ERASE_DISPLAY        ESCAPE_CODE("2J")
			#define ESC_ERASE_LINE           ESCAPE_CODE("K")

#endif
