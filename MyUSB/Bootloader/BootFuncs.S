;
;             MyUSB Library
;     Copyright (C) Dean Camera, 2008.
;              
;  dean [at] fourwalledcubicle [dot] com
;      www.fourwalledcubicle.com
;
; Released under the LGPL Licence, Version 3
;

#include "BootFuncs.h"

.global boot_read_sig_byte
.func boot_read_sig_byte
.endfunc
boot_read_sig_byte:
	out IO_ADDR(RAMPZ), r1       ; Clear Z Ramp via r1 _ZERO_ register
	clr ZH                       ; Clear ZH
	mov ZL, r24                  ; Set ZL to the passed in sig address
	ldi r18, ((1 << SIGRD) | (1 << SPMEN))
	out IO_ADDR(SPMCSR), r18     ; Set the LPM mode
	lpm                          ; Read in the sig bytes
	mov r24, r0                  ; Set the return value
	ret                          ; Return to calling function

.global wtd_disable_at90usb1287
.func wtd_disable_at90usb1287
.endfunc
wtd_disable_at90usb1287:
	wdr

	in r18, IO_ADDR(MCUSR)
	andi r18, ~(1 << WDRF)
	out IO_ADDR(MCUSR), r18

	lds r18, MEM_ADDR(WDTCSR)
	ori r18, ((1 << WDCE) | (1 << WDE))
	sts MEM_ADDR(WDTCSR), r18
	
	ldi r18, (0 << WDE)
	sts MEM_ADDR(WDTCSR), r18

	ret
