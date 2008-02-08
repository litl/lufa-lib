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

.global ReadSigByte
.func ReadSigByte
.endfunc
ReadSigByte:
	out IO_ADDR(RAMPZ), r1       ; Clear Z Ramp via r1 _ZERO_ register
	clr ZH                       ; Clear ZH
	mov ZL, r24                  ; Set ZL to the passed in sig address
	ldi r18, ((1 << SIGRD) | (1 << SPMEN))
	out IO_ADDR(SPMCSR), r18     ; Set the LPM mode
	lpm                          ; Read in the sig bytes
	mov r24, r0                  ; Set the return value
	rjmp WaitForSPMENCleared     ; Wait until SPMEN cleared, ready for next operation

WaitForSPMENCleared:
	in r18, IO_ADDR(SPMCSR)      ; Load in current SPMCSR register value
	sbrc r18, SPMEN              ; Skip to the ret if SPMEN cleared
	rjmp WaitForSPMENCleared     ; Jump back to loop start if not cleared
	ret                          ; Return to calling function
