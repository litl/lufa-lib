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
	clr r18                      ; Clear temp register
	out IO_ADDR(RAMPZ), r18      ; Clear Z Ramp via temp register
	clr ZH                       ; Clear ZH
	mov ZL, r24                  ; Set ZL to the passed in sig address
	ldi r18, ((1 << SIGRD) | (1 << SPMEN))
	out IO_ADDR(SPMCSR), r18     ; Set the LPM mode
	lpm                          ; Read in the sig bytes
	mov r24, r0                  ; Set the return value
	ret                          ; Return to calling function
