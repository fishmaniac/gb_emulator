#include "cpu.h"
#include <stdint.h>
#include <stdio.h>

static const uint8_t ZERO_FLAG_BIT_POS = 7;
static const uint8_t SUBTRACT_FLAG_BIT_POS = 6;
static const uint8_t HALF_CARRY_FLAG_BIT_POS = 5;
static const uint8_t CARRY_FLAG_BIT_POS = 4;
static const uint8_t FLAG_BIT_3_POS = 3;
static const uint8_t FLAG_BIT_2_POS = 2;
static const uint8_t FLAG_BIT_1_POS = 1;


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

void init_cpu(CPU *cpu) {
	cpu->pc = 0;
	set_sp_addr(cpu, 0xFFFE);
	// cpu->sp = &cpu->map.memory[0xFFFE];

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
	cpu->halt = 0;
	cpu->ime = 0;
}

void power_up_sequence(CPU *cpu) {

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
	uint8_t or = cpu->reg.a & val;

	cpu->reg.f.zero = or == 0;
	cpu->reg.f.subtract = false;
	cpu->reg.f.carry = false;
	cpu->reg.f.half_carry =
		((cpu->reg.a & UINT4_MAX) 
		| (val & UINT4_MAX)) 
		> UINT4_MAX;

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

	cpu->reg.a = cp;
}
void inc(CPU *cpu, uint8_t val) {
	uint8_t inc = cpu->reg.a + 1;

	cpu->reg.f.zero = inc == 0;
	cpu->reg.f.subtract = false;
	// Carry not affected
	// TODO: Half carry set on carry from bit 3
	cpu->reg.f.half_carry = false;

	cpu->reg.a = inc;
}
void dec(CPU *cpu, uint8_t val) {
	uint8_t inc = cpu->reg.a - 1;

	cpu->reg.f.zero = inc == 0;
	cpu->reg.f.subtract = true;
	// Carry not affected
	// TODO: Half carry set on no borrow from bit 4
	cpu->reg.f.half_carry = false;

	cpu->reg.a = inc;
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
		// TODO
		case AF:
			//TODO
			/* return (uint16_t*) &cpu->reg.a; */
			return NULL;
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
		// TODO: case 6, HL
		case 7:
			return A;
		default:
			printf("[ERROR]\tInvalid decode register 8 bit\n");
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
			printf("[ERROR]\tInvalid decode register 16 bit - AF\n");
		default:
			printf("[ERROR]\tInvalid decode register 16 bit: %d\n", reg);
	}
}

// Remove return values from instructions, instead set directly
// Instead, Decode opcodes in this function and call appropriately
void execute_instruction(CPU *cpu, Instruction opcode, Target_8 target_8, Target_16 target_16, int bit_index, int address) {
	uint8_t *target_reg_8 = get_target_reg_8(cpu, target_8);
	uint16_t *target_reg_16 = get_target_reg_16(cpu, target_16);
	printf("Executing instruction: %d = %s dest, src(%d - %d)\n", opcode, instruction_string(opcode), *target_reg_8, *target_reg_16);

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
char *instruction_string(Instruction opcode) {
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
