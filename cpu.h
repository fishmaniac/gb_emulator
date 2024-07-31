#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "constants.h"
#include "memory.h"

typedef struct FlagRegister_t {
	bool zero;		// Z
	bool subtract;		// N
	bool half_carry;	// H
	bool carry;		// C
} FlagRegister;

uint8_t get_reg_f(FlagRegister *flags);
void set_reg_f(FlagRegister *flags, uint8_t val);

#pragma pack(1)
typedef struct Register_t {
	uint8_t a;
	FlagRegister f;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
} Register;

typedef struct CPU_t {
	Register reg;
	MemoryMap map;
	bool ime;	// Interrupt Master Enable Flag	TODO: Set to false when ROM loaded
	bool halt;
} CPU;

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
	AF,	// ALU Accumulator / Flags Register
	BC,
	DE,
	HL,
	SP,
	PC,
} Target_16;

uint16_t get_reg_af(Register *reg);
void set_reg_af(Register *reg, uint16_t val);

uint16_t get_reg_bc(Register *reg);
void set_reg_bc(Register *reg, uint16_t val);

uint16_t get_reg_de(Register *reg);
void set_reg_de(Register *reg, uint16_t val);

uint16_t get_reg_hl(Register *reg);
void set_reg_hl(Register *reg, uint16_t val);

uint16_t get_reg_sp(Register *reg);
void set_reg_sp(Register *reg, uint16_t val);

uint16_t get_reg_pc(Register *reg);
void set_reg_pc(Register *reg, uint16_t val);

uint8_t get_16_high(uint16_t num);
uint8_t get_16_low(uint16_t num);
void increment_reg_16(CPU *cpu, Target_16 target);
void decrement_reg_16(CPU *cpu, Target_16 target);

typedef enum {
	ADC,
	ADD,
	ADDHL,
	ADDSP,
	AND,
	ANDHL,
	BIT,
	CALL,
	DI,
	EI,
	POP,
	PUSH,
} Instruction;


// typedef struct {
// 	void (*ADC_FN) (CPU, uint8_t);
// 	void (*ADD_FN) (CPU, uint8_t);
// } Opcode;

void ld_r8_r8(CPU *cpu, Target_8 target, Target_8 contents);
void ld_r8_n8(CPU *cpu, Target_8 target, uint8_t val);
void ld_r8_hl(CPU *cpu, Target_8 target);
void ld_hl_r8(CPU *cpu, Target_8 target);
void ld_hl_n8(CPU *cpu, uint8_t val);
void push(CPU *cpu, Target_16 target);
void pop(CPU *cpu, Target_16 target);
void adc(CPU *cpu, uint8_t val);
void add_r8(CPU *cpu, Target_8 target);
void add_n8(CPU *cpu, uint8_t val);
void addhl(CPU *cpu, uint16_t val);
void addsp(CPU *cpu, uint8_t val);
void and(CPU *cpu, uint8_t val);
void andhl(CPU *cpu, uint16_t val);
void bit(CPU *cpu, uint8_t val, int bit);
void call(CPU *cpu, int n);
void di(CPU *cpu);
void ei(CPU *cpu);

uint8_t *get_target_reg_8(CPU *cpu, Target_8 target);
uint16_t *get_target_reg_16(CPU *cpu, Target_16 target);

// void execute_instruction(CPU *cpu, Instruction opcode, Target_8 target_8, Target_16 target_16, int bit_index, int address);

#endif	// CPU_H
