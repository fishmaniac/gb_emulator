#include "cpu.h"
#include <stdio.h>

const uint8_t ZERO_FLAG_BIT_POS = 7;
const uint8_t SUBTRACT_FLAG_BIT_POS = 6;
const uint8_t HALF_CARRY_FLAG_BIT_POS = 5;
const uint8_t CARRY_FLAG_BIT_POS = 4;

uint8_t get_reg_f(FlagRegister *flags) {
	return (flags->zero << ZERO_FLAG_BIT_POS
	| flags->subtract << SUBTRACT_FLAG_BIT_POS
	| flags->half_carry << HALF_CARRY_FLAG_BIT_POS
	| flags->carry << CARRY_FLAG_BIT_POS);
}

void set_reg_f(FlagRegister *flags, uint8_t val) {
	flags->zero = (val >> ZERO_FLAG_BIT_POS) & 1;
	flags->subtract = (val >> SUBTRACT_FLAG_BIT_POS) & 1;
	flags->half_carry = (val >> HALF_CARRY_FLAG_BIT_POS) & 1;
	flags->carry = (val >> CARRY_FLAG_BIT_POS) & 1;
}

uint16_t get_reg_af(Register *reg) {
	return ((uint16_t) reg->a) << 8 | ((uint16_t) get_reg_f(&reg->f));
}


void set_reg_af(Register *reg, uint16_t val) {
	reg->a = (uint8_t) ((val & 0xFF00) >> 8);
	set_reg_f(&reg->f, (uint8_t) (val & UINT8_MAX));
}

uint16_t get_reg_bc(Register *reg) {
	return ((uint16_t) reg->b) << 8 | ((uint16_t) reg->c);
}

void set_reg_bc(Register *reg, uint16_t val) {
	reg->b = (uint8_t) ((val & 0xFF00) >> 8);
	reg->c = (uint8_t) (val & UINT8_MAX);
}

uint16_t get_reg_de(Register *reg) {
	return ((uint16_t) reg->d) << 8 | ((uint16_t) reg->e);
}

void set_reg_de(Register *reg, uint16_t val) {
	reg->d = (uint8_t) ((val & 0xFF00) >> 8);
	reg->e = (uint8_t) (val & UINT8_MAX);
}

uint16_t get_reg_hl(Register *reg) {
	return ((uint16_t) reg->h) << 8 | ((uint16_t) reg->l);
}

void set_reg_hl(Register *reg, uint16_t val) {
	reg->h = (uint8_t) ((val & 0xFF00) >> 8);
	reg->l = (uint8_t) (val & UINT8_MAX);
}

// 8 bit add to A register
uint8_t add(CPU *cpu, uint8_t val) {
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = val + cpu->reg.a > UINT8_MAX;
	cpu->reg.f.half_carry = (cpu->reg.a & UINT4_MAX) + (val & UINT4_MAX) > UINT4_MAX;
	return cpu->reg.a + val;
}

// 16 bit add to HL register
uint16_t addhl(CPU *cpu, uint8_t val) {
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = val + get_reg_hl(&cpu->reg) > UINT16_MAX;
	cpu->reg.f.half_carry = (get_reg_hl(&cpu->reg) & UINT12_MAX) + (val & UINT12_MAX) > UINT12_MAX;
	return get_reg_hl(&cpu->reg) + val;
}

// 8 bit add to A register with carry
uint8_t addc(CPU *cpu, uint8_t val) {
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = val + cpu->reg.a + cpu->reg.f.carry > UINT8_MAX;
	// Unsure if half carry is correct
	cpu->reg.f.half_carry = (cpu->reg.a & UINT4_MAX) + (val & UINT4_MAX) + cpu->reg.f.carry > UINT4_MAX;
	return cpu->reg.a + val + cpu->reg.f.carry;
}

// Get 8 bit register
// TODO get 16 bit register
// TODO get flag register
uint8_t *get_target_reg(CPU *cpu, Target target) {
	switch (target) {
		case A:
			return &cpu->reg.a;
		case B:
			return &cpu->reg.b;
		case C:
			return &cpu->reg.c;
		case D:
			return &cpu->reg.d;
		case E:
			return &cpu->reg.e;
		case F:
			// TODO
			return NULL;
		case G:
			return &cpu->reg.g;
		case H:
			return &cpu->reg.h;
		case L:
			return &cpu->reg.l;
		default:
			return NULL;
	}
}

void execute_opcode(CPU *cpu, Opcode opcode, Target target) {
	uint8_t *target_reg = get_target_reg(cpu, target);;
	switch (opcode) {
		// TODO implement 16 bit add
		case ADD:
			printf("ADD\n");
			uint8_t v = add(cpu, *target_reg);
			cpu->reg.a = v;
			break;
		case ADDHL:
			printf("ADDHL\n");
			uint16_t g = addhl(cpu, *target_reg);
			set_reg_hl(&cpu->reg, g);
			break;
		case ADC:
			printf("ADC\n");
			uint8_t c = addc(cpu, *target_reg);
			cpu->reg.a = c;
			break;
	}
}
