#include "cpu.h"
#include <stdio.h>

const uint8_t ZERO_FLAG_BIT_POS = 7;
const uint8_t SUBTRACT_FLAG_BIT_POS = 6;
const uint8_t HALF_CARRY_FLAG_BIT_POS = 5;
const uint8_t CARRY_FLAG_BIT_POS = 4;

uint8_t get_reg_F(FlagRegister *flags) {
	return (flags->zero << ZERO_FLAG_BIT_POS
	| flags->subtract << SUBTRACT_FLAG_BIT_POS
	| flags->half_carry << HALF_CARRY_FLAG_BIT_POS
	| flags->carry << CARRY_FLAG_BIT_POS);
}

void set_reg_F(FlagRegister *flags, uint8_t val) {
	flags->zero = (val >> ZERO_FLAG_BIT_POS) & 1;
	flags->subtract = (val >> SUBTRACT_FLAG_BIT_POS) & 1;
	flags->half_carry = (val >> HALF_CARRY_FLAG_BIT_POS) & 1;
	flags->carry = (val >> CARRY_FLAG_BIT_POS) & 1;
}

uint16_t get_reg_AF(Register *reg) {
	return ((uint16_t) reg->a) << 8 | ((uint16_t) get_reg_F(&reg->f));
}


void set_reg_AF(Register *reg, uint16_t val) {
	reg->a = (uint8_t) ((val & 0xFF00) >> 8);
	set_reg_F(&reg->f, (uint8_t) (val & 0xFF));
}

uint16_t get_reg_BC(Register *reg) {
	return ((uint16_t) reg->b) << 8 | ((uint16_t) reg->c);
}

void set_reg_BC(Register *reg, uint16_t val) {
	reg->b = (uint8_t) ((val & 0xFF00) >> 8);
	reg->c = (uint8_t) (val & 0xFF);
}

uint16_t get_reg_DE(Register *reg) {
	return ((uint16_t) reg->d) << 8 | ((uint16_t) reg->e);
}

void set_reg_DE(Register *reg, uint16_t val) {
	reg->d = (uint8_t) ((val & 0xFF00) >> 8);
	reg->e = (uint8_t) (val & 0xFF);
}

uint16_t get_reg_HL(Register *reg) {
	return ((uint16_t) reg->h) << 8 | ((uint16_t) reg->l);
}

void set_reg_HL(Register *reg, uint16_t val) {
	reg->h = (uint8_t) ((val & 0xFF00) >> 8);
	reg->l = (uint8_t) (val & 0xFF);
}
