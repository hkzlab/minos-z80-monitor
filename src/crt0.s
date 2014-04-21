;; Custom CRT0 for ITHACA AUDIO Z80 CPU monitor
;; Taken from SDCC

	.module crt0
	.globl	_main

	.globl  l__INITIALIZER
	.globl  s__INITIALIZED
 	.globl  s__INITIALIZER

	.area	_HEADER (ABS)
	;; Reset vector
	.org 	0xF000
	jp	init
	.org	0xF008
	reti
	.org	0xF010
	reti
	.org	0xF018
	reti
	.org	0xF020
	reti
	.org	0xF028
	reti
	.org	0xF030
	reti
	.org	0xF038
	reti
	.org	0xF040
init:
	;; Stack at the top of memory.
	ld	sp,#0xEFFF

    ;; Initialise global variables
    call	gsinit
	call	_main
	jp		_exit

	;; Ordering of segments for the linker.
	.area	_HOME
	.area	_CODE
	.area	_INITIALIZER
	.area   _GSINIT
	.area   _GSFINAL

	.area	_DATA
	.area	_INITIALIZED
	.area	_BSEG
	.area   _BSS
	.area   _HEAP

	.area   _CODE
__clock::
	ld	a,#2
	rst     0x08
	ret

_exit::
	;; Exit - special code to the emulator
	ld	a,#0
	rst     0x08
1$:
	halt
	jr	1$

	.area   _GSINIT
gsinit::
	ld	bc, #l__INITIALIZER
	ld	a, b
	or	a, c
	jr	Z, gsinit_next
	ld	de, #s__INITIALIZED
	ld	hl, #s__INITIALIZER
	ldir
gsinit_next:

	.area   _GSFINAL
	ret

