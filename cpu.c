#include "cpu.h"
#include <stdint.h>
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

uint8_t get_16_high(uint16_t num) {
	return num >> 8; // Higher 8 bits
}
uint8_t get_16_low(uint16_t num) {
	return num & 0xFF; // Lower 8 bits
}

void increment_reg_16(CPU *cpu, Target_16 target) {
	// TODO: Handle null
	uint16_t *_16 = get_target_reg_16(cpu, target);
	_16++;
}
void decrement_reg_16(CPU *cpu, Target_16 target) {
	// TODO: Handle null
	uint16_t *_16 = get_target_reg_16(cpu, target);
	_16--;
}

// 8 bit load register into target
void ld_r8_r8(CPU *cpu, Target_8 target, Target_8 contents) {
	uint8_t *contents_8 = get_target_reg_8(cpu, contents);
	uint8_t *target_8 = get_target_reg_8(cpu, target);
	*target_8 = *contents_8;
}

// 8 bit load n8 into target
void ld_r8_n8(CPU *cpu, Target_8 target, uint8_t val) {
	uint8_t *_8 = get_target_reg_8(cpu, target);
	*_8 = val;
}
// 8 bit load memory at addr HL into target
void ld_r8_hl(CPU *cpu, Target_8 target) {
	uint8_t *_8 = get_target_reg_8(cpu, target);
	uint16_t addr = get_reg_hl(&cpu->reg);
	uint8_t val = read_memory(&cpu->map, addr);
	*_8 = val;
}
// Store contents into memory at addr HL
void ld_hl_r8(CPU *cpu, Target_8 contents) {
	uint8_t *_8 = get_target_reg_8(cpu, contents);
	uint16_t addr = get_reg_hl(&cpu->reg);
	write_memory(&cpu->map, addr, *_8);
}
// Store val into memory at addr HL
void ld_hl_n8(CPU *cpu, uint8_t val) {
	uint16_t addr = get_reg_hl(&cpu->reg);
	write_memory(&cpu->map, addr, val);
}
// Can replace r8_hl
// Is actually defined as BC and DE separately
void ld_r8_r16(CPU *cpu, Target_8 target, Target_16 contents) {
	uint8_t *_8 = get_target_reg_8(cpu, target);
	uint16_t *addr = get_target_reg_16(cpu, contents);
	uint8_t val = read_memory(&cpu->map, *addr);
	*_8 = val;
}
// TODO: ld_a_c, ld_c_a, ld_a_n8, ld_n8_a, ld_a_n16, ld_n16_a, ld_a_hli, ld_a_hld, ld_bc_a, ld_de_a, ld_hli_a, ld_hld_a, ld_r16_n16, ld_sp_hl
void push(CPU *cpu, Target_16 target) {
	// TODO: Handle null
	uint16_t *reg = get_target_reg_16(cpu, target);

	// Move to previous position in memory
	decrement_reg_16(cpu, SP);
	// Get high bits
	uint8_t bits = get_16_high(*reg);
	uint16_t addr = get_reg_sp(&cpu->reg);
	write_memory(
		&cpu->map,
		addr,
		bits
	);
	// Move to previous position in memory
	decrement_reg_16(cpu, SP);
	// Get high bits
	bits = get_16_low(*reg);
	addr = get_reg_sp(&cpu->reg);
	write_memory(
		&cpu->map,
		get_reg_sp(&cpu->reg),
		bits
	);
}
void pop(CPU *cpu, Target_16 target) {
	// TODO: Handle null
	uint16_t *reg = get_target_reg_16(cpu, target);

	uint8_t mem_at_addr_sp = read_memory(
		&cpu->map,
		get_reg_sp(&cpu->reg)
	);
	// Get high bits
	uint8_t low = get_16_high(*reg);
	// Preserve high bits and write low bits
	uint16_t val = low << 8 | mem_at_addr_sp;
	*reg = val; // Load first memory block to target
	increment_reg_16(cpu, SP); // Move to next position in memory

	mem_at_addr_sp = read_memory(
		&cpu->map,
		get_reg_sp(&cpu->reg)
	);
	// Get low bits
	low = get_16_low(*reg);
	// Preserve low bits and write high bits
	val = mem_at_addr_sp << 8 | low;
	*reg = val; // Load second memory block to target
	increment_reg_16(cpu, SP); // Move to next position in memory
}
// TODO: ldhl_sp_e, ld_n16_sp
// 8 bit add r8 to A register
void add_r8(CPU *cpu, Target_8 target) {
	uint8_t val = *get_target_reg_8(cpu, target);
	add_n8(cpu, val);
}
// 8 bit add n8 to A register
void add_n8(CPU *cpu, uint8_t val) {
	uint8_t add = cpu->reg.a + val;

	cpu->reg.f.zero = add == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = cpu->reg.a + val > UINT8_MAX;
	cpu->reg.f.half_carry =
		(cpu->reg.a & UINT4_MAX)
		+ (val & UINT4_MAX)
		> UINT4_MAX;

	cpu->reg.a = add;
}
// 8 bit add memory at addr stored in hl to A
void add_hl(CPU *cpu) {
	uint8_t addr = get_reg_hl(&cpu->reg);
	uint8_t val = read_memory(&cpu->map, addr);

	add_n8(cpu, val);
}
// TODO: adc_a_s, adc_a_r, adc_a_n, adc_a_hl
// 8 bit add to A register with carry
void adc(CPU *cpu, uint8_t val) {
	// TODO: FIXME
	uint8_t adc = cpu->reg.a + val + cpu->reg.f.carry;

	cpu->reg.f.zero = adc == 0;
	cpu->reg.f.subtract = false;
	// Should carry be set before or after?
	cpu->reg.f.carry = cpu->reg.a + val + cpu->reg.f.carry > UINT8_MAX;
	// Can be simplified to (adc & UINT4_MAX) > UINT4_MAX?
	cpu->reg.f.half_carry =
		(cpu->reg.a & UINT4_MAX)
		+ (val & UINT4_MAX)
		+ cpu->reg.f.carry
		> UINT4_MAX;

	cpu->reg.a = adc;
}
// val accepts BC, DE, HL, and SP
// 16 bit add to HL register
void addhl(CPU *cpu, uint16_t val) {
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = val + get_reg_hl(&cpu->reg) > UINT16_MAX;
	cpu->reg.f.half_carry =
		(get_reg_hl(&cpu->reg) & UINT12_MAX)
		+ (val & UINT12_MAX)
		> UINT12_MAX;
	uint16_t hl = get_reg_hl(&cpu->reg) + val;
	set_reg_hl(&cpu->reg, hl);
}

// 16 bit add to SP register
void addsp(CPU *cpu, uint8_t val) {
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = val + get_reg_sp(&cpu->reg) > UINT16_MAX;
	cpu->reg.f.half_carry =
		(get_reg_sp(&cpu->reg) & UINT12_MAX)
		+ (val & UINT12_MAX)
		> UINT12_MAX;
	uint16_t sp = get_reg_sp(&cpu->reg) + val;
	set_reg_sp(&cpu->reg, sp);
}

// 8 bit and val with A register
void and(CPU *cpu, uint8_t val) {
	uint8_t and = cpu->reg.a & val;

	cpu->reg.f.zero = and == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = and > UINT8_MAX;
	cpu->reg.f.half_carry =
		((cpu->reg.a & UINT4_MAX) 
		& (val & UINT4_MAX)) 
		> UINT4_MAX;

	cpu->reg.a = and;
}
void andhl(CPU *cpu, uint16_t val) {
	
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
uint8_t *get_target_reg_8(CPU *cpu, Target_8 target) {
	switch (target) {
		case A:
			return &cpu->reg.a;
		case F:
			// TODO
			return NULL;
		case B:
			return &cpu->reg.b;
		case C:
			return &cpu->reg.c;
		case D:
			return &cpu->reg.d;
		case E:
			return &cpu->reg.e;
		case H:
			return &cpu->reg.h;
		case L:
			return &cpu->reg.l;
		default:
			// TODO: Error logger
			printf("[ERROR]\tINVALID TARGET REG 8 BIT\n");
			return NULL;
	}
}
uint16_t *get_target_reg_16(CPU *cpu, Target_16 target) {
	switch (target) {
		// TODO
		case AF:
			//TODO
			/* return (uint16_t*) &cpu->reg.a; */
			return NULL;
		case BC:
			return (uint16_t*) &cpu->reg.b;
		case DE:
			return (uint16_t*) &cpu->reg.d;
		case HL:
			return (uint16_t*) &cpu->reg.h;
		case SP:
			return &cpu->reg.sp;
		case PC:
			return &cpu->reg.pc;
		default:
			// TODO: Error logger
			printf("[ERROR]\tINVALID TARGET REG 16 BIT\n");
			return NULL;
	}
}

// Remove return values from instructions, instead set directly
// Instead, Decode opcodes in this function and call appropriately
// void execute_instruction(CPU *cpu, Instruction opcode, Target_8 target_8, Target_16 target_16, int bit_index, int address) {
// 	uint8_t *target_reg_8 = get_target_reg_8(cpu, target_8);
// 	uint16_t *target_reg_16 = get_target_reg_16(cpu, target_16);
//
// 	switch (opcode) {
// 		case ADC:
// 			adc(cpu, *target_reg_8);
// 			break;
// 		case ADD:
// 			add_n8(cpu, *target_reg_8);
// 			break;
// 		case ADDHL:
// 			addhl(cpu, *target_reg_16);
// 			break;
// 		case ADDSP:
// 			addsp(cpu, *target_reg_8);
// 			break;
// 		case AND:
// 			and(cpu, *target_reg_8);
// 			break;
// 		case BIT:
// 			bit(cpu, *target_reg_8, bit_index);
// 			break;
// 		case CALL:
// 			call(cpu, address);
// 			break;
// 		case DI:
// 			di(cpu);
// 			break;
// 		case EI:
// 			ei(cpu);
// 			break;
// 		case POP:
// 			pop(cpu, *target_reg_16);
// 			break;
// 		case PUSH:
// 			push(cpu, *target_reg_16);
// 			break;
// 	}
// }
