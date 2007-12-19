/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef __TERMINALCODES_H__
#define __TERMINALCODES_H__

	/* Public Interface - May be used in end-application: */
		/* Defines: */
			#define ANSI_ESCAPE_SEQUENCE(c)  "\33[" c

			#define ESC_RESET                ANSI_ESCAPE_SEQUENCE("0m")
			#define ESC_BOLD_ON              ANSI_ESCAPE_SEQUENCE("1m")
			#define ESC_ITALIC_ON            ANSI_ESCAPE_SEQUENCE("3m")
			#define ESC_UNDERLINE_ON         ANSI_ESCAPE_SEQUENCE("4m")
			#define ESC_INVERSE_ON           ANSI_ESCAPE_SEQUENCE("7m")
			#define ESC_STRIKETHROUGH_ON     ANSI_ESCAPE_SEQUENCE("9m")
			#define ESC_BOLD_OFF             ANSI_ESCAPE_SEQUENCE("22m")
			#define ESC_ITALICS_OFF          ANSI_ESCAPE_SEQUENCE("23m")
			#define ESC_UNDERLINE_OFF        ANSI_ESCAPE_SEQUENCE("24m")
			#define ESC_INVERSE_OFF          ANSI_ESCAPE_SEQUENCE("27m")
			#define ESC_STRIKETHROUGH_OFF    ANSI_ESCAPE_SEQUENCE("29m")
			#define ESC_FG_BLACK             ANSI_ESCAPE_SEQUENCE("30m")
			#define ESC_FG_RED               ANSI_ESCAPE_SEQUENCE("31m")
			#define ESC_FG_GREEN             ANSI_ESCAPE_SEQUENCE("32m")
			#define ESC_FG_YELLOW            ANSI_ESCAPE_SEQUENCE("33m")
			#define ESC_FG_BLUE              ANSI_ESCAPE_SEQUENCE("34m")
			#define ESC_FG_MAGENTA           ANSI_ESCAPE_SEQUENCE("35m")
			#define ESC_FG_CYAN              ANSI_ESCAPE_SEQUENCE("36m")
			#define ESC_FG_WHITE             ANSI_ESCAPE_SEQUENCE("37m")
			#define ESC_FG_DEFAULT           ANSI_ESCAPE_SEQUENCE("39m")
			#define ESC_BG_BLACK             ANSI_ESCAPE_SEQUENCE("40m")
			#define ESC_BG_RED               ANSI_ESCAPE_SEQUENCE("41m")
			#define ESC_BG_GREEN             ANSI_ESCAPE_SEQUENCE("42m")
			#define ESC_BG_YELLOW            ANSI_ESCAPE_SEQUENCE("43m")
			#define ESC_BG_BLUE              ANSI_ESCAPE_SEQUENCE("44m")
			#define ESC_BG_MAGENTA           ANSI_ESCAPE_SEQUENCE("45m")
			#define ESC_BG_CYAN              ANSI_ESCAPE_SEQUENCE("46m")
			#define ESC_BG_WHITE             ANSI_ESCAPE_SEQUENCE("47m")
			#define ESC_BG_DEFAULT           ANSI_ESCAPE_SEQUENCE("49m")
			
			#define ESC_CURSOR_POS(L, C)     ANSI_ESCAPE_SEQUENCE(#L ";" #C "H")
			#define ESC_CURSOR_UP(L)         ANSI_ESCAPE_SEQUENCE(#L "A")
			#define ESC_CURSOR_DOWN(L)       ANSI_ESCAPE_SEQUENCE(#L "B")
			#define ESC_CURSOR_FORWARD(C)    ANSI_ESCAPE_SEQUENCE(#C "C")
			#define ESC_CURSOR_BACKWARD(C)   ANSI_ESCAPE_SEQUENCE(#C "D")
			#define ESC_CURSOR_POS_SAVE      ANSI_ESCAPE_SEQUENCE("s")
			#define ESC_CURSOR_POS_RESTORE   ANSI_ESCAPE_SEQUENCE("u")
			
			#define ESC_ERASE_DISPLAY        ANSI_ESCAPE_SEQUENCE("2J")
			#define ESC_ERASE_LINE           ANSI_ESCAPE_SEQUENCE("K")

#endif
