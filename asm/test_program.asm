SECTION "header placeholder", ROM0[$0100]
	nop
	jp main
	ds 76, $00
SECTION "Test program", ROM0
main:
	ld (0x1000), sp
	halt
