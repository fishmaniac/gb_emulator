#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

typedef struct FlagRegister_t {
	bool carry;		// C
	bool half_carry;	// H
	bool subtract;		// N
	bool zero;		// Z
	bool bit_0;
	bool bit_1;
	bool bit_2;
	bool bit_3;
} FlagRegister;

#pragma pack(1)
typedef struct Register_t {
	uint8_t c;
	uint8_t b;
	uint8_t e;
	uint8_t d;
	uint8_t l;
	uint8_t h;
	uint8_t a;
	FlagRegister f;
} Register;

typedef union WordUnion_t {
	uint8_t *bytes[2];
	uint16_t *word;
} WordUnion;

typedef struct CPU_t {
	Register reg;
	MemoryMap map;
	uint16_t pc;
	uint16_t sp;
	bool ime;
	bool ime_next;
	bool halt;
} CPU;

void init_cpu(CPU *cpu);

uint16_t read_immediate_16(CPU *cpu);
uint8_t read_immediate_8(CPU *cpu);
int8_t read_signed_immediate_8(CPU *cpu);

uint8_t get_reg_f(FlagRegister *flags);
void set_reg_f(FlagRegister *flags, uint8_t val);

uint16_t get_sp_addr(CPU *cpu);
uint8_t *get_sp_mem(CPU *cpu);
uint8_t get_sp_val(CPU *cpu);
void set_sp_addr(CPU *cpu, uint16_t addr);
void set_sp_val(CPU *cpu, uint8_t val);

typedef enum {
	A,	// ALU Accumulator
	F,
	B,
	C,
	D,
	E,
	H,
	L,
} Target_8;

typedef enum {
	BC,
	DE,
	HL,
	SP,
	PC,
	AF,	// ALU Accumulator / Flags Register
} Target_16;

// Maybe remove all get and set
uint16_t get_reg_af(Register *reg);
void set_reg_af(Register *reg, uint16_t val);

uint16_t get_reg_bc(Register *reg);
void set_reg_bc(Register *reg, uint16_t val);

uint16_t get_reg_de(Register *reg);
void set_reg_de(Register *reg, uint16_t val);

uint16_t get_reg_hl(Register *reg);
void set_reg_hl(Register *reg, uint16_t val);

uint16_t get_pc(CPU *cpu);
void set_pc(CPU *cpu, uint16_t val);

uint8_t get_16_high(uint16_t num);
uint8_t get_16_low(uint16_t num);
void increment_reg_16(CPU *cpu, Target_16 target);
void decrement_reg_16(CPU *cpu, Target_16 target);

typedef enum {
	ADD,
	ADC,
	// ADDHL,	// temp removed (at ADD + 6)
	// ADDSP,	// temp removed (at 0xE0 + 8) ??
	SUB,
	SBC,
	AND,
	// ANDHL,	// temp removed (at AND + 6)
	XOR,
	OR,
	CP,
	// INC,
	// DEC,
	// CALL,
	// DI,
	// EI,
	// POP,
	// PUSH,
} Instruction;


// typedef struct {
// 	void (*ADC_FN) (CPU, uint8_t);
// 	void (*ADD_FN) (CPU, uint8_t);
// } Opcode;

// void ld_r8_n8(CPU *cpu, Target_8 target, uint8_t val);
// void ld_r8_r8(CPU *cpu, Target_8 target, Target_8 contents);
// void ld_r8_hl(CPU *cpu, Target_8 target);
// void ld_hl_r8(CPU *cpu, Target_8 target);
// void ld_hl_n8(CPU *cpu, uint8_t val);
void push_n16(CPU *cpu, uint16_t val);
void push_r16(CPU *cpu, Target_16 target);
void pop_r16(CPU *cpu, Target_16 target);
uint16_t pop_n16(CPU *cpu);
void add_r8(CPU *cpu, Target_8 target);
void add_n8(CPU *cpu, uint8_t val);
void addhl(CPU *cpu, uint16_t val);
void addsp(CPU *cpu, uint8_t val);
void adc(CPU *cpu, uint8_t val);
// void andhl(CPU *cpu, uint16_t val);
void sub(CPU *cpu, uint8_t val);
void sbc(CPU *cpu, uint8_t val);
void and(CPU *cpu, uint8_t val);
void or(CPU *cpu, uint8_t val);
void xor(CPU *cpu, uint8_t val);
void cp(CPU *cpu, uint8_t val);
void inc(CPU *cpu);
void dec(CPU *cpu);
void call_n16(CPU *cpu, uint16_t n);
void call_a16(CPU *cpu);
void di(CPU *cpu);
void ei(CPU *cpu);
void jp(CPU *cpu, uint16_t addr);

uint8_t *get_target_reg_8(CPU *cpu, Target_8 target);
uint16_t *get_target_reg_16(CPU *cpu, Target_16 target);

Target_8 decode_register_8_pairs(int reg, bool lower);
Target_8 decode_register_8(int reg);
Target_16 decode_register_16(int reg);

int execute_instruction(CPU *cpu);
void execute_math_instruction(CPU *cpu, Instruction opcode, Target_8 target_8, Target_16 target_16, int bit_index, int address);

char *instruction_string(Instruction opcode);

#endif	// CPU_H
