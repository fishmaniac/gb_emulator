#include "cpu.h"
#include <stdio.h>

static const uint8_t ZERO_FLAG_BIT_POS = 7;
static const uint8_t SUBTRACT_FLAG_BIT_POS = 6;
static const uint8_t HALF_CARRY_FLAG_BIT_POS = 5;
static const uint8_t CARRY_FLAG_BIT_POS = 4;

uint8_t get_reg_f(FlagRegister *flags) {
	return flags->zero << ZERO_FLAG_BIT_POS
	| flags->subtract << SUBTRACT_FLAG_BIT_POS
	| flags->half_carry << HALF_CARRY_FLAG_BIT_POS
	| flags->carry << CARRY_FLAG_BIT_POS;
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

uint16_t get_reg_sp(Register *reg) {
	return reg->sp;
}
void set_reg_sp(Register *reg, uint16_t val) {
	reg->sp = val;
}

uint16_t get_reg_pc(Register *reg) {
	return reg->pc;
}
void set_reg_pc(Register *reg, uint16_t val) {
	reg->pc = val;
}

// 8 bit add to A register with carry
uint8_t adc(CPU *cpu, uint8_t val) {
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = val + cpu->reg.a + cpu->reg.f.carry > UINT8_MAX;
	// Unsure if half carry is correct
	cpu->reg.f.half_carry = (cpu->reg.a & UINT4_MAX) + (val & UINT4_MAX) + cpu->reg.f.carry > UINT4_MAX;
	return cpu->reg.a + val + cpu->reg.f.carry;
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

// 16 bit add to SP register
uint16_t addsp(CPU *cpu, uint8_t val) {
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = val + get_reg_sp(&cpu->reg) > UINT16_MAX;
	cpu->reg.f.half_carry = (get_reg_sp(&cpu->reg) & UINT12_MAX) + (val & UINT12_MAX) > UINT12_MAX;
	return get_reg_sp(&cpu->reg) + val;
}

// 8 bit and val with A
uint8_t and(CPU *cpu, uint8_t val) {
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = (val & cpu->reg.a) > UINT8_MAX;
	cpu->reg.f.half_carry = ((cpu->reg.a & UINT4_MAX) & (val & UINT4_MAX)) > UINT4_MAX;

	return cpu->reg.a & val;
}
// TODO 16 bit
// Test bit in register val, set zero flag accordingly
// bit is 3 bits (0-7)
void bit(CPU *cpu, uint8_t val, int bit) {
	cpu->reg.f.zero = ((val >> bit) & 1) == 0;
	cpu->reg.f.subtract = false;
	// Carry not affecteed
	// TODO half carry
	cpu->reg.f.half_carry = false;
}
// n is 2 byte, LS byte first
void call(CPU *cpu, int n) {
	// TODO: push address of next instruction to stack and jump to address n
	// Decode opcode at n + 1, execute it
}
//Disable interrupts
void di(CPU *cpu) {
	cpu->ime = false;
}
//Enable interrupts
void ei(CPU *cpu) {
	// TODO: Delay setting to true by one instruction
	cpu->ime = true;
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
		case SP:
			// TODO
			return NULL;
		case PC:
			// TODO
			return NULL;
		case NONE:
			return NULL;
		default:
			return NULL;
	}
}

// b is 3 bit target
// n is 2 byte address
void execute_opcode(CPU *cpu, Instruction opcode, Target target, int bit_index, int address) {
	uint8_t _8 = 0;
	uint16_t _16 = 0;
	uint8_t *target_reg = get_target_reg(cpu, target);

	switch (opcode) {
		case ADC:
			_8 = adc(cpu, *target_reg);
			cpu->reg.a = _8;
			break;
		case ADD:
			_8 = add(cpu, *target_reg);
			cpu->reg.a = _8;
			break;
		case ADDHL:
			_16 = addhl(cpu, *target_reg);
			set_reg_hl(&cpu->reg, _16);
			break;
		case ADDSP:
			_16 = addsp(cpu, *target_reg);
			set_reg_sp(&cpu->reg, _16);
			break;
		case AND:
			_8 = and(cpu, *target_reg);
			set_reg_sp(&cpu->reg, _8);
			break;
		case BIT:
			bit(cpu, *target_reg, bit_index);
			break;
		case CALL:
			call(cpu, address);
			break;
		case DI:
			di(cpu);
			break;
		case EI:
			ei(cpu);
			break;
	}
}
