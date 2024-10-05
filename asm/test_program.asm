SECTION "header placeholder", ROM0[$0100]
	nop
	jp main
	ds 76, $00
SECTION "Test program", ROM0
main:
	add a, b
	halt
