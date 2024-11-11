#include "cpu.h"
#include "constants.h"
#include "timer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const uint8_t ZERO_FLAG_BIT_POS = 7;
static const uint8_t SUBTRACT_FLAG_BIT_POS = 6;
static const uint8_t HALF_CARRY_FLAG_BIT_POS = 5;
static const uint8_t CARRY_FLAG_BIT_POS = 4;
static const uint8_t FLAG_BIT_3_POS = 3;
static const uint8_t FLAG_BIT_2_POS = 2;
static const uint8_t FLAG_BIT_1_POS = 1;

void init_cpu(CPU *cpu) {
	for (int i = 0; i < MEMORY_SIZE; i++) {
		cpu->map.memory[i] = 0;
	}

	cpu->pc = 0x100;
	set_sp_addr(cpu, 0xFFFE);
	cpu->ime = false;
	cpu->halt = false;
	cpu->ime_next = false;

	cpu->reg.a = 0;
	cpu->reg.b = 0;
	cpu->reg.c = 0;
	cpu->reg.d = 0;
	cpu->reg.e = 0;
	cpu->reg.h = 0;
	cpu->reg.l = 0;

	cpu->reg.f.carry = 0;
	cpu->reg.f.half_carry = 0;
	cpu->reg.f.subtract = 0;
	cpu->reg.f.zero = 0;
	cpu->reg.f.bit_3 = 0;
	cpu->reg.f.bit_2 = 0;
	cpu->reg.f.bit_1 = 0;
	cpu->reg.f.bit_0 = 0;
}

uint16_t read_immediate_16(CPU *cpu) {
	uint8_t right = read_memory(&cpu->map, cpu->pc);
	uint16_t left = read_memory(&cpu->map, cpu->pc + 1) << 8;
	uint16_t im = right | left;
	cpu->pc += 2;
	printf("Read immediate 16 = 0x%X, PC: %d\n", im, cpu->pc);
	return im;
}
uint8_t read_immediate_8(CPU *cpu) {
	uint8_t im = read_memory(&cpu->map, cpu->pc);
	cpu->pc++;
	printf("Read immediate 8 = 0x%X, PC: %d\n", im, cpu->pc);
	return im;
}
int8_t read_signed_immediate_8(CPU *cpu) {
	int8_t im = (int8_t) read_memory(&cpu->map, cpu->pc);
	cpu->pc++;
	printf("Read signed immediate 8 = 0x%X, PC: %d\n", im, cpu->pc);
	return im;
}

uint8_t get_reg_f(FlagRegister *flags) {
	return flags->zero << ZERO_FLAG_BIT_POS
	| flags->subtract << SUBTRACT_FLAG_BIT_POS
	| flags->half_carry << HALF_CARRY_FLAG_BIT_POS
	| flags->carry << CARRY_FLAG_BIT_POS
	| flags->bit_3 << FLAG_BIT_3_POS
	| flags->bit_2 << FLAG_BIT_2_POS
	| flags->bit_1;
}
void set_reg_f(FlagRegister *flags, uint8_t val) {
	flags->zero = (val >> ZERO_FLAG_BIT_POS) & 1;
	flags->subtract = (val >> SUBTRACT_FLAG_BIT_POS) & 1;
	flags->half_carry = (val >> HALF_CARRY_FLAG_BIT_POS) & 1;
	flags->carry = (val >> CARRY_FLAG_BIT_POS) & 1;
	flags->bit_3 = (val >> FLAG_BIT_3_POS) & 1;
	flags->bit_2 = (val >> FLAG_BIT_2_POS) & 1;
	flags->bit_1 = (val >> FLAG_BIT_1_POS) & 1;
	flags->bit_0 = val & 1;
}

uint16_t get_sp_addr(CPU *cpu) {
	return cpu->sp;
}
uint8_t *get_sp_mem(CPU *cpu) {
	return &cpu->map.memory[cpu->sp];
}
uint8_t get_sp_val(CPU *cpu) {
	return read_memory(&cpu->map, cpu->sp);
}
void set_sp_addr(CPU *cpu, uint16_t addr) {
	cpu->sp = addr;
}
void set_sp_val(CPU *cpu, uint8_t val) {
	write_memory(&cpu->map, cpu->sp, val);
}

uint16_t get_pc(CPU *cpu) {
	return cpu->pc;
}

void set_pc(CPU *cpu, uint16_t val) {
	cpu->pc = val;
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

uint8_t get_16_high(uint16_t num) {
	return (num >> 8) & 0xFF;
}

uint8_t get_16_low(uint16_t num) {
	return num & 0xFF;
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
void increment_reg_8(CPU *cpu, Target_8 target) {
	// TODO: Handle null
	uint8_t *_8 = get_target_reg_8(cpu, target);
	_8++;
}
void decrement_reg_8(CPU *cpu, Target_8 target) {
	// TODO: Handle null
	uint8_t *_8 = get_target_reg_8(cpu, target);
	_8--;
}
// // 8 bit load n8 into target
// void ld_r8_n8(CPU *cpu, Target_8 target, uint8_t val) {
// 	uint8_t *_8 = get_target_reg_8(cpu, target);
// 	*_8 = val;
// }
// // 8 bit load register into target
// void ld_r8_r8(CPU *cpu, Target_8 target, Target_8 contents) {
// 	uint8_t contents_8 = *get_target_reg_8(cpu, contents);
// 	ld_r8_n8(cpu, target, contents_8);
// }
// // Is actually defined as BC and DE separately
// //
// // 8 bit load memory at addr contents into target
// void ld_r8_r16(CPU *cpu, Target_8 target, Target_16 contents) {
// 	uint16_t *addr = get_target_reg_16(cpu, contents);
// 	printf("addr HL: %d\ttarget: %d\n", *addr, contents);
// 	uint8_t val = read_memory(&cpu->map, *addr);
// 	printf("HL val: %d\tval: %d\n", *addr, val);
// 	ld_r8_n8(cpu, target, val);
// }
// // 8 bit load memory at addr HL into target
// void ld_r8_hl(CPU *cpu, Target_8 target) {
// 	ld_r8_r16(cpu, target, HL);
// }
// // Store contents into memory at addr HL
// void ld_hl_r8(CPU *cpu, Target_8 contents) {
// 	uint8_t *_8 = get_target_reg_8(cpu, contents);
// 	uint16_t addr = get_reg_hl(&cpu->reg);
// 	write_memory(&cpu->map, addr, *_8);
// }
// // Store val into memory at addr HL
// void ld_hl_n8(CPU *cpu, uint8_t val) {
// 	uint16_t addr = get_reg_hl(&cpu->reg);
// 	write_memory(&cpu->map, addr, val);
// }

// TODO: ld_a_c, ld_c_a, ld_a_n8, ld_n8_a, ld_a_n16, ld_n16_a, ld_a_hli, ld_a_hld, ld_bc_a, ld_de_a, ld_hli_a, ld_hld_a, ld_r16_n16, ld_sp_hl
void push_n16(CPU *cpu, uint16_t val) {
	cpu->sp--;
	set_sp_val(cpu, val >> 8);
	cpu->sp--;
	set_sp_val(cpu, val & 0xFF);
}
void push_r16(CPU *cpu, Target_16 target) {
	uint16_t reg = *get_target_reg_16(cpu, target);

	cpu->sp--;
	set_sp_val(cpu, reg >> 8);
	cpu->sp--;
	set_sp_val(cpu, reg & 0xFF);
}
void pop_r16(CPU *cpu, Target_16 target) {
	uint16_t *reg = get_target_reg_16(cpu, target);

	uint8_t low = get_sp_val(cpu);
	cpu->sp++;
	uint8_t high = get_sp_val(cpu);
	cpu->sp++;

	uint16_t bits = low | (high << 8);

	*reg = bits;
}
uint16_t pop_n16(CPU *cpu) {
	uint8_t low = get_sp_val(cpu);
	cpu->sp++;
	uint8_t high = get_sp_val(cpu);
	cpu->sp++;

	uint16_t bits = low | (high << 8);

	return bits;
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
	cpu->reg.f.carry = val + *get_sp_mem(cpu) > UINT16_MAX;
	cpu->reg.f.half_carry =
		(*get_sp_mem(cpu) & UINT12_MAX)
		+ (val & UINT12_MAX)
		> UINT12_MAX;
	uint8_t sp = *get_sp_mem(cpu) + val;
	set_sp_addr(cpu, sp);
}
// TODO: adc_a_s, adc_a_r, adc_a_n, adc_a_hl
// 8 bit add to A register with carry
void adc(CPU *cpu, uint8_t val) {
	// TODO: FIXME
	uint8_t adc = cpu->reg.a + val + cpu->reg.f.carry;
	if (cpu->reg.a + val > UINT8_MAX) adc++;

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
// void andhl(CPU *cpu, uint16_t val) {
// 	
// }
void sub(CPU *cpu, uint8_t val) {
	uint8_t sub = cpu->reg.a - val;

	cpu->reg.f.zero = sub == 0;
	cpu->reg.f.subtract = true;

	// TODO: Carry flags set on for no borrow
	// Half carry set on for no borrow from bit 4
	cpu->reg.f.carry = cpu->reg.a + val > UINT8_MAX;
	cpu->reg.f.half_carry =
		(cpu->reg.a & UINT4_MAX)
		+ (val & UINT4_MAX)
		> UINT4_MAX;

	cpu->reg.a = sub;
}
void sbc(CPU *cpu, uint8_t val) {
	sub(cpu, val);

	cpu->reg.a += cpu->reg.f.carry;
}
// 8 bit and val with A register
void and(CPU *cpu, uint8_t val) {
	uint8_t and = cpu->reg.a & val;

	cpu->reg.f.zero = and == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = false;
	cpu->reg.f.half_carry =
		((cpu->reg.a & UINT4_MAX) 
		& (val & UINT4_MAX)) 
		> UINT4_MAX;

	cpu->reg.a = and;
}
void or(CPU *cpu, uint8_t val) {
	uint8_t or = cpu->reg.a | val;

	cpu->reg.f.zero = or == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = false;
	cpu->reg.f.half_carry = false;

	cpu->reg.a = or;
}
void xor(CPU *cpu, uint8_t val) {
	uint8_t xor = cpu->reg.a ^ val;

	cpu->reg.f.zero = xor == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = false;
	cpu->reg.f.half_carry = false;

	cpu->reg.a = xor;
}
void cp(CPU *cpu, uint8_t val) {
	bool cp = cpu->reg.a == val;

	cpu->reg.f.zero = cp;
	cpu->reg.f.subtract = false;
	// TODO: Carry flags set on for no borrow
	// Half carry set on for no borrow from bit 4
	cpu->reg.f.carry = false;
	cpu->reg.f.half_carry = false;

	// Reg A is unaffected?
	// cpu->reg.a = cp;
}
void inc(CPU *cpu) {
	uint8_t inc = cpu->reg.a + 1;

	cpu->reg.f.zero = inc == 0;
	cpu->reg.f.subtract = false;
	// Carry not affected
	// TODO: Half carry set on carry from bit 3
	cpu->reg.f.half_carry = false;

	cpu->reg.a = inc;
}
void dec(CPU *cpu) {
	uint8_t dec = cpu->reg.a - 1;

	cpu->reg.f.zero = dec == 0;
	cpu->reg.f.subtract = true;
	// Carry not affected
	// TODO: Half carry set on no borrow from bit 4
	cpu->reg.f.half_carry = false;

	cpu->reg.a = dec;
}

// CALL, n16
void call_n16(CPU *cpu, uint16_t n) {
	push_n16(cpu, cpu->pc);
	cpu->pc = n;
}
// CALL, a16
void call_a16(CPU *cpu) {
	push_n16(cpu, cpu->pc);
	cpu->pc = read_immediate_16(cpu);
}
// Disable interrupts
void di(CPU *cpu) {
	cpu->ime = false;
}
// Enable interrupts
void ei(CPU *cpu) {
	// TODO: Delay setting to true by one instruction
	cpu->ime = true;
}
// Jump to addr
void jp(CPU *cpu, uint16_t addr) {
	cpu->pc = addr;
}
void daa(CPU *cpu) {
	bool carry = false;
	if (!cpu->reg.f.subtract) {
		if (cpu->reg.f.carry || cpu->reg.a > 0x99) {
			cpu->reg.a += 0x60;
			carry = true;
		}
		if (cpu->reg.f.half_carry || (cpu->reg.a & 0xf) > 0x9) {
			cpu->reg.a += 0x6;
		}
	}
	else if (cpu->reg.f.carry) {
		carry = true;
		if (cpu->reg.f.half_carry) {
			cpu->reg.a += 0x9A;
		}
		else {
			cpu->reg.a += 0xA0;
		}
	}
	else if (cpu->reg.f.half_carry) {
		cpu->reg.a += 0xFA;
	}

	cpu->reg.f.zero = cpu->reg.a == 0;
	cpu->reg.f.half_carry = 0;
	cpu->reg.f.carry = carry;
}
void rlc(CPU *cpu, Target_8 target) {
	uint8_t reg = *get_target_reg_8(cpu, target);
	if (reg == 0) cpu->reg.f.carry = false;
	else cpu->reg.f.carry = cpu->reg.a >> 7;

	uint8_t val = cpu->reg.a << reg;
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.half_carry = false;
	cpu->reg.f.subtract = false;

	cpu->reg.a = val;

}
void rrc(CPU *cpu, Target_8 target) {
	uint8_t reg = *get_target_reg_8(cpu, target);
	if (reg == 0) cpu->reg.f.carry = false;
	else cpu->reg.f.carry = cpu->reg.a & 0x1;

	uint8_t val = cpu->reg.a << reg;
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.half_carry = false;
	cpu->reg.f.subtract = false;

	cpu->reg.a = val;
}
void rl(CPU *cpu, Target_8 target) {
	uint8_t bit_7 = cpu->reg.a >> 7;

	uint8_t reg = *get_target_reg_8(cpu, target);
	if (reg == 0) cpu->reg.f.carry = false;
	else cpu->reg.f.carry = cpu->reg.a >> 7;

	uint8_t val = cpu->reg.a << reg;
	val |= bit_7 << 7;
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.half_carry = false;
	cpu->reg.f.subtract = false;

	cpu->reg.a = val;
}
void rr(CPU *cpu, Target_8 target) {
	uint8_t bit_0 = cpu->reg.a & 0x1;

	uint8_t reg = *get_target_reg_8(cpu, target);
	if (reg == 0) cpu->reg.f.carry = false;
	else cpu->reg.f.carry = cpu->reg.a & 0x1;

	uint8_t val = cpu->reg.a << reg;
	val |= bit_0;
	cpu->reg.f.zero = val == 0;
	cpu->reg.f.half_carry = false;
	cpu->reg.f.subtract = false;

	cpu->reg.a = val;
}
void swap(CPU *cpu, Target_8 target) {
	uint8_t *reg = get_target_reg_8(cpu, target);
	uint8_t left = (*reg) >> 4;
	uint8_t right = ((*reg) & 0xF) << 4;
	*reg = left | right;
}

// Test bit in register val, set zero flag accordingly
// bit is 3 bits (0-7)
void bit(CPU *cpu, Target_8 target, int bit) {
	uint8_t val = *get_target_reg_8(cpu, target);
	cpu->reg.f.zero = ((val >> bit) & 1) == 0;
	cpu->reg.f.subtract = false;
	// Carry not affecteed
	// TODO half carry
	cpu->reg.f.half_carry = false;
}


// Get 8 bit register
uint8_t *get_target_reg_8(CPU *cpu, Target_8 target) {
	switch (target) {
		case A:
			return &cpu->reg.a;
		case F:
			return (uint8_t*) &cpu->reg.f.carry;
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
// TODO: Fix endianness bug
uint16_t *get_target_reg_16(CPU *cpu, Target_16 target) {
	switch (target) {
		case BC:
			return (uint16_t*) &cpu->reg.c;
		case DE:
			return (uint16_t*) &cpu->reg.e;
		case HL:
			return (uint16_t*) &cpu->reg.l;
		case PC:
			return &cpu->pc;
		case SP:
			return &cpu->sp;
		case AF:
			return (uint16_t*) &cpu->reg.a;
		default:
			// TODO: Error logger
			printf("[ERROR]\tINVALID TARGET REG 16 BIT: %d\n", target);
			return NULL;
	}
}

Target_8 decode_register_8_pairs(int reg, bool lower) {
	if (lower) {
		switch(reg) {
			case 0:
				return B;
			case 1:
				return D;
			case 2:
				return H;
			default:
				printf("[ERROR]\tInvalid decode register 8 bit pairs lower\n");
				return -1;
		}
	}
	else {
		switch(reg) {
			case 0:
				return C;
			case 1:
				return E;
			case 2:
				return L;
			case 3:
				return A;
			default:
				printf("[ERROR]\tInvalid decode register 8 bit pairs higher\n");
				return -1;
		}
	}
}

Target_8 decode_register_8(int reg) {
	switch(reg) {
		case 0:
			return B;
		case 1:
			return C;
		case 2:
			return D;
		case 3:
			return E;
		case 4:
			return H;
		case 5:
			return L;
		case 7:
			return A;
		default:
			printf("[ERROR]\tInvalid decode register 8 bit\n");
			return -1;
	}
}

Target_16 decode_register_16(int reg) {
	switch (reg) {
		case 0:
			return BC;
		case 1:
			return DE;
		case 2:
			return HL;
		case 3:
			return AF;
		default:
			printf("[ERROR]\tInvalid decode register 16 bit: %d\n", reg);
			return -1;
	}
}
void execute_cb_instruction(CPU *cpu, CBInstruction instruction, Target_8 target) {
	printf("Executing CB: opcode: %d, target: %d\n", instruction, target);
	// TODO: Decode opcode to instruction
	// Sets of 2 instructions per 15 (opcode & 0xF0) values
	// if ((opcode & 0xF) > 0x7) opcode++;
	if ((instruction & 0xF) == 0x6 || (instruction & 0xF) == 0xE) {
		printf("[ERROR] - Unimplemented 0xCB, 0xX6 and 0xXE = 0x%x\n", instruction);
		exit(3);
	}
	else {
		switch (instruction) {
			case RLC:
				rlc(cpu, target);
				break;
			case RRC:
				rrc(cpu, target);
				break;
			case RL:
				rl(cpu, target);
				break;
			case RR:
				rr(cpu, target);
				break;
			case SLA:
				printf("[ERROR] - Unimplemented 0xCB SLA - Target: %d\n", target);
				// sla(cpu, target);
				break;
			case SRA:
				printf("[ERROR] - Unimplemented 0xCB SRA - Target: %d\n", target);
				break;
			case SWAP:
				swap(cpu, target);
				break;
			case SRL:
				printf("[ERROR] - Unimplemented 0xCB SRL - Target: %d\n", target);
				// srl(cpu, target);
				break;
			case BIT0:
				bit(cpu, target, 0);
				break;
			case BIT1:
				bit(cpu, target, 1);
				break;
			case BIT2:
				bit(cpu, target, 2);
				break;
			case BIT3:
				bit(cpu, target, 3);
				break;
			case BIT4:
				bit(cpu, target, 4);
				break;
			case BIT5:
				bit(cpu, target, 5);
				break;
			case BIT6:
				bit(cpu, target, 6);
				break;
			case BIT7:
				bit(cpu, target, 7);
				break;
			case RES0:
				// break;
			case RES1:
				// break;
			case RES2:
				// break;
			case RES3:
				// break;
			case RES4:
				// break;
			case RES5:
				// break;
			case RES6:
				// break;
			case RES7:
				printf("[ERROR] - Unimplemented 0xCB RES - Target: %d\n", target);
				break;
			case SET0:
				// break;
			case SET1:
				// break;
			case SET2:
				// break;
			case SET3:
				// break;
			case SET4:
				// break;
			case SET5:
				// break;
			case SET6:
				// break;
			case SET7:
				printf("[ERROR] - Unimplemented 0xCB SET - Target: %d\n", target);
				break;
		}
	}
		// switch (opcode) {
		// 	case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07:
		// 		rlc(cpu, target_8);
		// 		break;
		// 	case 0x06:
		// 		printf("[ERROR] - Unimplemented RLC HL 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0f:
		// 		rrc(cpu, target_8);
		// 		break;
		// 	case 0x0e:
		// 		printf("[ERROR] - Unimplemented RRC HL 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x17:
		// 		rl(cpu, target_8);
		// 		break;
		// 	case 0x16:
		// 		printf("[ERROR] - Unimplemented RL HL 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1f:
		// 		rr(cpu, target_8);
		// 		break;
		// 	case 0x1e:
		// 		printf("[ERROR] - Unimplemented RR HL 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x27:
		// 		printf("[ERROR] - Unimplemented SLA 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x26:
		// 		printf("[ERROR] - Unimplemented SLA HL 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2e: case 0x2f:
		// 		printf("[ERROR] - Unimplemented SRA 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37:
		// 		swap(cpu, target_8);
		// 		break;
		// 	case 0x36:
		// 		printf("[ERROR] - Unimplemented SWAP HL 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		// 		printf("[ERROR] - Unimplemented SRL 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		// 		printf("[ERROR] - Unimplemented BIT 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
		// 		printf("[ERROR] - Unimplemented BIT 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
		// 		printf("[ERROR] - Unimplemented BIT 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5e: case 0x5f:
		// 		printf("[ERROR] - Unimplemented BIT 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
		// 		printf("[ERROR] - Unimplemented BIT 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6e: case 0x6f:
		// 		printf("[ERROR] - Unimplemented BIT 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
		// 		printf("[ERROR] - Unimplemented BIT 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xc0: case 0xc1: case 0xc2: case 0xc3: case 0xc4: case 0xc5: case 0xc6: case 0xc7:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xc8: case 0xc9: case 0xca: case 0xcb: case 0xcc: case 0xcd: case 0xce: case 0xcf:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xd0: case 0xd1: case 0xd2: case 0xd3: case 0xd4: case 0xd5: case 0xd6: case 0xd7:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xd8: case 0xd9: case 0xda: case 0xdb: case 0xdc: case 0xdd: case 0xde: case 0xdf:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe6: case 0xe7:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xe8: case 0xe9: case 0xea: case 0xeb: case 0xec: case 0xed: case 0xee: case 0xef:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xf0: case 0xf1: case 0xf2: case 0xf3: case 0xf4: case 0xf5: case 0xf6: case 0xf7:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// 	case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xfe: case 0xff:
		// 		printf("[ERROR] - Unimplemented SET 0xCB 0x%x\n", opcode);
		// 		exit(3);
		// 		break;
		// }
}
int execute_instruction(CPU *cpu) {
	bool bit_7 = false;
	uint8_t *dest_8 = NULL;
	uint16_t *dest_16 = NULL;
	uint8_t val_8 = 0;

	uint8_t opcode = read_memory(&cpu->map, cpu->pc);
	if (&cpu->map.memory[cpu->pc] == NULL) {
		printf("[ERROR] - Null operation\n");
		exit(-4);
	}

	printf("\nPC: %d - 0x%X\tOP: 0x%x\n", cpu->pc, cpu->pc, opcode);
	printf("FLAGS\tFz: %d, Fs: %d, Fh: %d, Fc: %d, IME: %d - %d, Halt: %d\n", cpu->reg.f.zero, cpu->reg.f.subtract, cpu->reg.f.half_carry, cpu->ime_next, cpu->reg.f.carry, cpu->ime, cpu->halt);
	printf("REGISTERS_8\tA: %d - 0x%X, B: %d - 0x%X, C: %d - 0x%X, D: %d - 0x%X, E: %d - 0x%X, H: %d - 0x%X, L: %d - 0x%X\n", cpu->reg.a, cpu->reg.a, cpu->reg.b, cpu->reg.b, cpu->reg.c, cpu->reg.c, cpu->reg.d, cpu->reg.d, cpu->reg.e, cpu->reg.e, cpu->reg.h, cpu->reg.h, cpu->reg.l, cpu->reg.l);
	printf("REGISTERS_16\tAF: %d - 0x%X, BC: %d - 0x%X, DE: %d - 0x%X, HL: %d - 0x%X\n", get_reg_af(&cpu->reg), get_reg_af(&cpu->reg), get_reg_bc(&cpu->reg), get_reg_bc(&cpu->reg), get_reg_de(&cpu->reg), get_reg_de(&cpu->reg), get_reg_hl(&cpu->reg), get_reg_hl(&cpu->reg));
	printf("STACK TOP: 0x%X\n", cpu->map.memory[cpu->sp] | (cpu->map.memory[cpu->sp + 1] << 8));
	printf("LY mem[0xFF44]: %d\n", cpu->map.memory[0xFF44]);

	cpu->pc++;

	// cpu->map.memory[0xFF44] = 0x90;

	if (opcode == 0xCB) {
		uint8_t opcode = read_immediate_8(cpu);
		int src = opcode & 0x7; // Extract bits 0-2
		int operation = (opcode >> 3) & 0x7; // Extract bits 3-5

		Target_8 reg_src = decode_register_8(src);

		// Convert the operation into a CBInstruction enum
		CBInstruction instruction;

		switch (operation) {
			case 0: // RLC / RRC
				instruction = (opcode & 0xF8) == 0x00 ? RLC : RRC;
				break;
			case 1: // RL / RR
				instruction = (opcode & 0xF8) == 0x08 ? RL : RR;
				break;
			case 2: // SLA / SRA
				instruction = (opcode & 0xF8) == 0x10 ? SLA : SRA;
				break;
			case 3: // SWAP / SRL
				instruction = (opcode & 0xF8) == 0x18 ? SWAP : SRL;
				break;
		}

		// Handle BIT, RES, SET operations
		if ((opcode & 0xC0) == 0x40) { // BIT (0b01)
			instruction = BIT0 + src; // BIT0 to BIT7
		} else if ((opcode & 0xC0) == 0x80) { // RES (0b10)
			instruction = RES0 + src; // RES0 to RES7
		} else if ((opcode & 0xC0) == 0xC0) { // SET (0b11)
			instruction = SET0 + src; // SET0 to SET7
		}

		execute_cb_instruction(cpu, instruction, reg_src);
	}
	else if (opcode >= 0x00 && opcode < 0x40) {
		printf("OP: 0x00 - 0x40\n");
		switch (opcode) {
			// NOP
			case 0x00:
				printf("Executed NOP\n");
				break;
			// STOP
			case 0x10:
				// TODO: Stop mode
				write_memory(&cpu->map, DIV_ADDR, 0);
				read_immediate_8(cpu);
				cpu->halt = true;
				break;
			// JR NZ, e8
			case 0x20:
				if (!cpu->reg.f.zero) {
					cpu->pc += read_signed_immediate_8(cpu);
				}
				else {
					cpu->pc++;
				}
				break;
			case 0x30:
				if (!cpu->reg.f.carry) {
					cpu->pc += read_signed_immediate_8(cpu);
				}
				else {
					cpu->pc++;
				}
				break;
			// LD r16, nn
			case 0x01: case 0x11: case 0x21: case 0x31:
				dest_16 = get_target_reg_16(cpu, opcode >> 4);
				*dest_16 = read_immediate_16(cpu);
				break;
			// LD r16, A
			case 0x02: case 0x12:
				dest_16 = get_target_reg_16(cpu, opcode >> 4);
				*dest_16 = cpu->reg.a;
				break;
			// LD (HL+),A
			case 0x22:
				dest_16 = get_target_reg_16(cpu, HL);
				write_memory(&cpu->map, *dest_16, cpu->reg.a);
				(*dest_16)++;
				break;
			// LD (HL-), A
			case 0x32:
				dest_16 = get_target_reg_16(cpu, HL);
				write_memory(&cpu->map, *dest_16, cpu->reg.a);
				(*dest_16)--;
				break;
			// INC r16
			case 0x03: case 0x13: case 0x23: case 0x33:
				dest_16 = get_target_reg_16(cpu, opcode >> 4);
				cpu->reg.f.half_carry = *dest_16 == 0xFF;
				cpu->reg.f.carry = *dest_16 == 0xFFFF;
				(*dest_16)++;
				cpu->reg.f.zero = *dest_16 == 0;
				cpu->reg.f.subtract = false;
				break;
			// INC B - INC D - INC H
			case 0x04: case 0x14: case 0x24:
				// TODO: Flags
				dest_8 = get_target_reg_8(
					cpu,
					decode_register_8_pairs(opcode >> 4, true)
				);
				cpu->reg.f.half_carry = *dest_8 == 0xF;
				cpu->reg.f.carry = *dest_8 == 0xFF;
				(*dest_8)++;
				cpu->reg.f.zero = *dest_8 == 0;
				cpu->reg.f.subtract = false;
				// TODO: All INC and DEC half carry
				break;
			// INC HL
			case 0x34:
				dest_16 = get_target_reg_16(cpu, HL);
				cpu->reg.f.half_carry = *dest_16 == 0xFF;
				cpu->reg.f.carry = *dest_16 == 0xFFFF;
				(*dest_16)++;
				cpu->reg.f.zero = *dest_16 == 0;
				cpu->reg.f.subtract = false;
				break;
			// DEC B - DEC D - DEC H
			case 0x05: case 0x15: case 0x25:
				// TODO: Flags
				dest_8 = get_target_reg_8(
					cpu,
					decode_register_8_pairs(opcode >> 4, true)
				);
				cpu->reg.f.half_carry = *dest_8 == 0xF;
				cpu->reg.f.carry = *dest_8 == 0xFF;
				(*dest_8)--;
				cpu->reg.f.zero = *dest_8 == 0;
				cpu->reg.f.subtract = true;
				break;
			// DEC HL
			case 0x35:
				dest_16 = get_target_reg_16(cpu, HL);
				cpu->reg.f.half_carry = *dest_16 == 0xFF;
				cpu->reg.f.carry = *dest_16 == 0xFFFF;
				(*dest_16)--;
				cpu->reg.f.zero = *dest_16 == 0;
				cpu->reg.f.subtract = true;
				break;
			// LD B, n8 - LD D, n8 - LD H, n8
			case 0x06: case 0x16: case 0x26:
				dest_8 = get_target_reg_8(
					cpu,
					decode_register_8_pairs(opcode >> 4, true)
				);
				*dest_8 = read_immediate_8(cpu);
				break;
			// LD HL, n8
			case 0x36:
				dest_16 = get_target_reg_16(
					cpu,
					HL
				);
				*dest_16 = read_immediate_8(cpu);
				break;
			// RLCA
			case 0x07:
				cpu->reg.f.carry = (cpu->reg.a & 0xFF) != 0;

				val_8 = (cpu->reg.a << 1);
				if (cpu->reg.f.carry) val_8 |= 0x01;
				cpu->reg.a = val_8;

				cpu->reg.f.zero = val_8 == 0;
				cpu->reg.f.subtract = false;
				cpu->reg.f.half_carry = false;
				break;
			// RLA
			case 0x17:
				bit_7 = (cpu->reg.a & 0xFF) != 0;

				val_8 = (cpu->reg.a << 1);
				if (cpu->reg.f.carry) val_8 |= 0x01;
				if (bit_7) cpu->reg.f.carry = true;
				cpu->reg.a = val_8;

				cpu->reg.f.zero = val_8 == 0;
				cpu->reg.f.subtract = false;
				cpu->reg.f.half_carry = false;
				break;
			// DAA
			case 0x27:
				daa(cpu);
				break;
			// SCF
			case 0x37:
				cpu->reg.f.carry = true;
				cpu->reg.f.subtract = false;
				cpu->reg.f.half_carry = false;
				break;
			// LD a16, SP 
			case 0x08:
				write_memory(
					&cpu->map,
					read_immediate_16(cpu),
					cpu->sp
				);
				break;
			// JR e8
			case 0x18:
				cpu->pc += read_signed_immediate_8(cpu);
				break;
			// JR Z, e8
			case 0x28:
				if (cpu->reg.f.zero) {
					cpu->pc += read_signed_immediate_8(cpu);
				}
				else {
					cpu->pc++;
				}
				break;
			// JR C, e8
			case 0x38:
				if (cpu->reg.f.carry) {
					cpu->pc += read_signed_immediate_8(cpu);
				}
				else {
					cpu->pc++;
				}
				break;
			// ADD HL, r16
			case 0x09: case 0x19: case 0x29: case 0x39:
				dest_16 = get_target_reg_16(cpu, opcode >> 4);
				set_reg_hl(&cpu->reg, get_reg_hl(&cpu->reg) + (*dest_16));

				cpu->reg.f.subtract = false;
				cpu->reg.f.zero = get_reg_hl(&cpu->reg) == 0;
				// TODO: Set carry flags
				// cpu->reg.f.carry;
				// cpu->reg.f.half_carry;
				break;
			// LD A, BC - LD A, DE
			case 0x0A: case 0x1A:
				dest_16 = get_target_reg_16(cpu, opcode >> 4);
				cpu->reg.a = *dest_16;
				break;
			// LD A, (HL+)
			case 0x2A:
				dest_16 = get_target_reg_16(cpu, HL);
				val_8 = read_memory(&cpu->map, *dest_16);
				cpu->reg.a = val_8;
				(*dest_16)++;
				break;
			// LD A, (HL-)
			case 0x3A:
				dest_16 = get_target_reg_16(cpu, HL);
				val_8 = read_memory(&cpu->map, *dest_16);
				cpu->reg.a = val_8;
				(*dest_16)--;
				break;
			// DEC r16
			case 0x0B: case 0x1B: case 0x2B: case 0x3B:
				dest_16 = get_target_reg_16(cpu, opcode >> 4);
				cpu->reg.f.half_carry = *dest_16 == 0xFF;
				cpu->reg.f.carry = *dest_16 == 0xFFFF;
				(*dest_16)--;
				cpu->reg.f.zero = *dest_16 == 0;
				cpu->reg.f.subtract = true;
				break;
			// INC C - INC E - INC L - INC A
			case 0x0C: case 0x1C: case 0x2C: case 0x3C:
				dest_8 = get_target_reg_8(
					cpu,
					decode_register_8_pairs(opcode >> 4, false)
				);
				cpu->reg.f.half_carry = *dest_8 == 0xF;
				cpu->reg.f.carry = *dest_8 == 0xFF;
				(*dest_8)++;
				cpu->reg.f.zero = *dest_8 == 0;
				cpu->reg.f.subtract = false;
				break;
			// DEC C - DEC E - DEC L - DEC A
			case 0x0D: case 0x1D: case 0x2D: case 0x3D:
				dest_8 = get_target_reg_8(
					cpu,
					decode_register_8_pairs(opcode >> 4, false)
				);
				cpu->reg.f.half_carry = *dest_8 == 0xF;
				cpu->reg.f.carry = *dest_8 == 0xFF;
				(*dest_8)--;
				cpu->reg.f.zero = *dest_8 == 0;
				cpu->reg.f.subtract = true;
				break;
			// LD C, n8 - LD E, n8 - LD L, n8 - LD A, n8
			case 0x0E: case 0x1E: case 0x2E: case 0x3E:
				dest_8 = get_target_reg_8(
					cpu,
					decode_register_8_pairs(opcode >> 4, false)
				);
				*dest_8 = read_immediate_8(cpu);

				printf("SET DEST: 0x%X\n", *dest_8);
				break;
			// RRCA
			case 0x0F:
				printf("[ERROR] - Unimplemented 0x%x\n", opcode);
				exit(2);
				break;
			// RRA
			case 0x1F:
				printf("[ERROR] - Unimplemented 0x%x\n", opcode);
				exit(2);
				break;
			// CPL
			case 0x2F:
				cpu->reg.a = ~cpu->reg.a;
				break;
			// CCF
			case 0x3F:
				cpu->reg.f.carry = !cpu->reg.f.carry;
				cpu->reg.f.subtract = false;
				cpu->reg.f.half_carry = false;
				break;
		}
	}
	// Shortcut for load instructions
	else if (opcode >= 0x40 && opcode < 0x80) {
		printf("OP: 0x40 - 0x80\n");
		switch (opcode) {
			// HALT
			case 0x76:
				printf("Executing HALT\n");
				cpu->halt = true;
				break;
			// LD B, HL
			case 0x46:
				cpu->reg.b = read_memory(&cpu->map, get_reg_hl(&cpu->reg));
				break;
			// LD D, HL
			case 0x56:
				cpu->reg.d = read_memory(&cpu->map, get_reg_hl(&cpu->reg));
				break;
			// LD H, HL
			case 0x66:
				cpu->reg.h = read_memory(&cpu->map, get_reg_hl(&cpu->reg));
				break;
			// LD C, HL
			case 0x4E:
				cpu->reg.c = read_memory(&cpu->map, get_reg_hl(&cpu->reg));
				break;
			// LD E, HL
			case 0x5E:
				cpu->reg.e = read_memory(&cpu->map, get_reg_hl(&cpu->reg));
				break;
			// LD L, HL
			case 0x6E:
				cpu->reg.l = read_memory(&cpu->map, get_reg_hl(&cpu->reg));
				break;
			// LD A, HL
			case 0x7E:
				cpu->reg.a = read_memory(&cpu->map, get_reg_hl(&cpu->reg));
				break;
			case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75:
				{
					int src = opcode & 0xF;
					Target_8 reg_src = decode_register_8(src);
					uint8_t val = *get_target_reg_8(cpu, reg_src);
					write_memory(&cpu->map, get_reg_hl(&cpu->reg), val);
				}
				break;
			case 0x77:
				{
					uint8_t val = *get_target_reg_8(cpu, A);
					write_memory(&cpu->map, get_reg_hl(&cpu->reg), val);
				}
				break;
			case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // LD B, r8
			case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f: // LD C, r8
			case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // LD D, r8
			case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f: // LD E, r8
			case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // LD H, r8
			case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f: // LD L, r8
			case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f: // LD A, r8
				{
					int src = opcode & 0x7;
					int dest = (opcode >> 3) & 0x7;
					// TODO: case for reg HL, HALT
					Target_8 reg_src = decode_register_8(src);
					Target_8 reg_dest = decode_register_8(dest);

					*get_target_reg_8(cpu, reg_dest) = *get_target_reg_8(cpu, reg_src);
					printf("Executed LD r8, r8: opcode: 0x%x, acc: %d, src: %d - %d, dest: %d - %d\n\n", cpu->reg.a, opcode, src, *get_target_reg_8(cpu, reg_src), dest, *get_target_reg_8(cpu, reg_dest));
				}
				break;
		}
	}
	// Math instructions
	// Add, sub, and, or
	else if (opcode >= 0x80 && opcode < 0xC0) {
		printf("OP: 0x80 - 0xC0\n");
		if ((opcode & 0xF) == 0x6 || (opcode & 0xF) == 0xE) {
			switch (opcode) {
				// ADD A, HL
				case 0x86:
					add_n8(
						cpu,
						read_memory(&cpu->map, get_reg_hl(&cpu->reg))
					);
					break;
				// SUB A, HL
				case 0x96:
					sub(
						cpu,
						read_memory(&cpu->map, get_reg_hl(&cpu->reg))
					);
					break;
				// AND A, HL
				case 0xA6:
					and(
						cpu,
						read_memory(&cpu->map, get_reg_hl(&cpu->reg))
					);
					break;
				// OR A, HL
				case 0xB6:
					or(
						cpu,
						read_memory(&cpu->map, get_reg_hl(&cpu->reg))
					);
					break;
				// ADC A, HL
				case 0x8E:
					adc(
						cpu,
						read_memory(&cpu->map, get_reg_hl(&cpu->reg))
					);
					break;
				// SBC A, HL
				case 0x9E:
					sbc(
						cpu,
						read_memory(&cpu->map, get_reg_hl(&cpu->reg))
					);
					break;
				// XOR A, HL
				case 0xAE:
					xor(
						cpu,
						read_memory(&cpu->map, get_reg_hl(&cpu->reg))
					);
					break;
				// CP A, HL
				case 0xBE:
					cp(
						cpu,
						read_memory(&cpu->map, get_reg_hl(&cpu->reg))
					);
					break;
			}
		}
		else {
			int src = opcode & 0x7;
			int operation = (opcode >> 3) & 0x7;
			Target_8 reg_src = decode_register_8(src);

			execute_math_instruction(cpu, operation, reg_src);
			printf("Executed MATH: opcode: 0x%x, operation: %d, acc: %d, src: %d - %d\n", opcode, operation, cpu->reg.a, src, *get_target_reg_8(cpu, reg_src));
		}
	}
	else if (opcode >= 0xC0 && opcode <= 0xFF) {
		printf("OP: 0xc0 - 0xFF\n");
		switch (opcode) {
			// RET NZ
			case 0xC0:
				if (!cpu->reg.f.zero) {
					jp(cpu, pop_n16(cpu));
				}
				break;
			// RET NC
			case 0xD0:
				if (!cpu->reg.f.carry) {
					jp(cpu, pop_n16(cpu));
				}
				break;
			// LDH a8, A
			case 0xE0:
				write_memory(&cpu->map, 0xFF00 + read_immediate_8(cpu), cpu->reg.a);
				break;
			// LDH A, a8
			case 0xF0:
				cpu->reg.a = read_memory(&cpu->map, 0xFF00 + read_immediate_8(cpu));
				break;
			// POP r16
			case 0xC1: case 0xD1: case 0xE1:
				pop_r16(cpu, decode_register_16((opcode - 0xC0) >> 4));
				printf("POP R16: D: %d - 0x%x, E: %d - 0x%x\n", cpu->reg.d, cpu->reg.d, cpu->reg.e, cpu->reg.e);
				break;
			case 0xF1:
				set_reg_af(&cpu->reg, pop_n16(cpu));
				printf("POP AF: %d\n", get_reg_af(&cpu->reg));
				break;
			// JP NZ, a16
			case 0xC2:
				if (!cpu->reg.f.zero) {
					cpu->pc = read_immediate_16(cpu);
				}
				else {
					cpu->pc += 2;
				}
				break;
			// JP NC, a16
			case 0xD2:
				if (!cpu->reg.f.carry) {
					cpu->pc = read_immediate_16(cpu);
				}
				else {
					cpu->pc += 2;
				}
				break;
			// LD (C), A
			case 0xE2:
				write_memory(&cpu->map, 0xFF00 + cpu->reg.c, cpu->reg.a);
				break;
			// LD A, (C)
			case 0xF2:
				cpu->reg.a = read_memory(&cpu->map, 0xFF00 + cpu->reg.c);
				break;
			// JP a16
			case 0xC3:
				jp(cpu, read_immediate_16(cpu));
				break;
			// DI
			case 0xF3:
				cpu->ime = false;
				break;
			// CALL NZ, a16
			case 0xC4:
				if (!cpu->reg.f.zero) {
					call_a16(cpu);
				}
				break;
			// CALL NC, a16
			case 0xD4:
				if (!cpu->reg.f.carry) {
					call_a16(cpu);
				}
				break;
			// PUSH r16
			case 0xC5: case 0xD5: case 0xE5:
				push_r16(cpu, decode_register_16((opcode - 0xC0) >> 4));
				break;
			case 0xF5:
				push_n16(cpu, get_reg_af(&cpu->reg));
				printf("PUSH AF: %d\n", get_reg_af(&cpu->reg));
				break;
			// ADD A, n8
			case 0xC6: 
				add_n8(cpu, read_immediate_8(cpu));
				break;
			// SUB A, n8
			case 0xD6:
				sub(cpu, read_immediate_8(cpu));
				break;
			// AND A, n8
			case 0xE6:
				and(cpu, read_immediate_8(cpu));
				break;
			// OR A, n8
			case 0xF6:
				or(cpu, read_immediate_8(cpu));
				break;
			// RST $00
			case 0xC7:
				push_n16(cpu, cpu->pc);
				jp(cpu, 0x00);
				break;
			// RST $10
			case 0xD7:
				push_n16(cpu, cpu->pc);
				jp(cpu, 0x10);
				break;
			// RST $20
			case 0xE7:
				push_n16(cpu, cpu->pc);
				jp(cpu, 0x20);
				break;
			// RST $30
			case 0xF7:
				push_n16(cpu, cpu->pc);
				jp(cpu, 0x30);
				break;
			// RET Z
			case 0xC8:
				if (cpu->reg.f.zero) {
					jp(cpu, pop_n16(cpu));
				}
				break;
			// RET C
			case 0xD8:
				if (cpu->reg.f.carry) {
					jp(cpu, pop_n16(cpu));
				}
				break;
			// ADD SP, e8
			case 0xE8:
				cpu->sp += read_signed_immediate_8(cpu);
				break;
			// LD HL, SP + e8
			case 0xF8:
				set_reg_hl(&cpu->reg, cpu->sp + read_signed_immediate_8(cpu));
				break;
			// RET
			case 0xC9:
				jp(cpu, pop_n16(cpu));
				break;
			// RETI
			case 0xD9:
				jp(cpu, pop_n16(cpu));
				cpu->ime = true;
				break;
			// JP HL
			case 0xE9:
				jp(cpu, get_reg_hl(&cpu->reg));
				break;
			// LD SP, HL
			case 0xF9:
				cpu->sp = get_reg_hl(&cpu->reg);
				break;
			// JP Z, a16
			case 0xCA:
				if (cpu->reg.f.zero) {
					cpu->pc = read_immediate_16(cpu);
				}
				else {
					cpu->pc += 2;
				}
				break;
			// JP C, a16
			case 0xDA:
				if (cpu->reg.f.carry) {
					cpu->pc = read_immediate_16(cpu);
				}
				else {
					cpu->pc += 2;
				}
				break;
			// LD a16, A
			case 0xEA:
				write_memory(&cpu->map, read_immediate_16(cpu), cpu->reg.a);
				break;
			// LD A, a16
			case 0xFA:
				cpu->reg.a = read_memory(&cpu->map, read_immediate_16(cpu));
				break;
			// PREFIX for color gameboy opcodes
			case 0xCB:
				printf("REACHED 0xCB\n");
				printf("[ERROR] - Unimplemented 0x%x\n", opcode);
				exit(1);
				break;
			// EI
			case 0xFB:
				cpu->ime_next = true;
				break;
			// CALL Z, a16
			case 0xCC:
				if (cpu->reg.f.zero) {
					call_a16(cpu);
				}
				break;
			// CALL C, a16
			case 0xDC:
				if (cpu->reg.f.carry) {
					call_a16(cpu);
				}
				break;
			// CALL a16
			case 0xCD:
				call_a16(cpu);
				break;
			// ADC A, n8
			case 0xCE:
				adc(cpu, read_immediate_8(cpu));
				break;
			// SBC A, n8
			case 0xDE:
				sbc(cpu, read_immediate_8(cpu));
				break;
			// XOR A, n8
			case 0xEE:
				xor(cpu, read_immediate_8(cpu));
				break;
			// CP A, n8
			case 0xFE:
				// TODO: zero
				cp(cpu, read_immediate_8(cpu));
				break;
			// RST $08
			case 0xCF:
				push_n16(cpu, cpu->pc);
				jp(cpu, 0x08);
				break;
			// RST $18
			case 0xDF:
				push_n16(cpu, cpu->pc);
				jp(cpu, 0x18);
				break;
			// RST $28
			case 0xEF:
				push_n16(cpu, cpu->pc);
				jp(cpu, 0x28);
				break;
			// RST $38
			case 0xFF:
				push_n16(cpu, cpu->pc);
				jp(cpu, 0x38);
				break;
			// Unused instructions
			case 0xD3: case 0xE3: case 0xE4: case 0xF4: case 0xDB: case 0xEB: case 0xEC:
			case 0xFC: case 0xDD: case 0xED: case 0xFD:
				printf("[ERROR] - Unused instruction: 0x%X\n", opcode);
				break;
		}
	}
	else {
		printf("[ERROR] - Unknown instruction 0x%x\n", opcode);
	}
	printf("END\tPC: %d - 0x%X, OP: 0x%x, A: %d, MEM[PC]: 0x%X, DEST_8: 0x%X, DEST_16: 0x%X\n", cpu->pc, cpu->pc, opcode, cpu->reg.a, cpu->map.memory[cpu->pc], (dest_8 == NULL) ? -1 : *dest_8, (dest_16 == NULL) ? -1 : *dest_16);

	return -1;
}

void execute_math_instruction(CPU *cpu, MathInstruction opcode, Target_8 target_8) {
	uint8_t *target_reg_8 = get_target_reg_8(cpu, target_8);

	switch (opcode) {
		case ADD:
			add_n8(cpu, *target_reg_8);
			break;
		case ADC:
			adc(cpu, *target_reg_8);
			break;
		// case ADDHL:
		// 	addhl(cpu, *target_reg_16);
		// 	break;
		// case ADDSP:
		// 	addsp(cpu, *target_reg_8);
		// 	break;
		case SUB:
			sub(cpu, *target_reg_8);
			break;
		case SBC:
			sbc(cpu, *target_reg_8);
			break;
		case AND:
			and(cpu, *target_reg_8);
			break;
		case OR:
			or(cpu, *target_reg_8);
			break;
		case XOR:
			xor(cpu, *target_reg_8);
			break;
		case CP:
			cp(cpu, *target_reg_8);
			break;
		// case INC:
		// 	inc(cpu, *target_reg_8);
		// 	break;
		// case DEC:
		// 	dec(cpu, *target_reg_8);
		// 	break;
		// case CALL:
		// 	call(cpu, address);
		// 	break;
		// case DI:
		// 	di(cpu);
		// 	break;
		// case EI:
		// 	ei(cpu);
		// 	break;
		// case POP:
		// 	pop(cpu, *target_reg_16);
		// 	break;
		// case PUSH:
		// 	push(cpu, *target_reg_16);
		// 	break;
	}
}
// copy from execute_instruction then
// f:€ý5hvbyjwv$bpbireturn "f;€ý5i"jdd0w
char *instruction_string(MathInstruction opcode) {
	switch (opcode) {
		case ADD:
			return "ADD";
		case ADC:
			return "ADC";
		// case ADDHL:
		// case ADDSP:
		case SUB:
			return "SUB";
		case SBC:
			return "SBC";
		case AND:
			return "AND";
		case OR:
			return "OR";
		case XOR:
			return "XOR";
		case CP:
			return "CP";
		// case INC:
		// 	return "INC";
		// case DEC:
		// 	return "DEC";
		// case CALL:
		// 	return "CALL";
		// case DI:
		// 	return "DI";
		// case EI:
		// 	return "EI";
		// case POP:
		// 	return "POP";
		// case PUSH:
		// 	return "PUSH";
	}
}
