SECTION "header placeholder", ROM0[$0100]
	nop
	jp main
	ds 76, $00
SECTION "Test program", ROM0
main:
	ld b, 5
	ld c, 0
	push bc
	pop de
	halt
