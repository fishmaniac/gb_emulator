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

typedef struct Register_t {
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	FlagRegister f;
	uint8_t g;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
} Register;

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

typedef enum {
	ADC,
	ADD,
	ADDHL,
	ADDSP,
	AND,
	BIT,
	CALL,
	DI,
	EI,
} Instruction;

typedef enum {
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	L,
	SP,
	PC,
	NONE,
} Target;

typedef struct CPU_t {
	Register reg;
	MemoryMap map;
	bool ime;	// Interrupt Master Enable Flag	TODO: Set to false when ROM loaded
} CPU;

uint8_t adc(CPU *cpu, uint8_t val);
uint8_t add(CPU *cpu, uint8_t val);
uint16_t addhl(CPU *cpu, uint8_t val);
uint16_t addsp(CPU *cpu, uint8_t val);
uint8_t and(CPU *cpu, uint8_t val);
void bit(CPU *cpu, uint8_t val, int bit);
void call(CPU *cpu, int n);
void di(CPU *cpu);
void ei(CPU *cpu);

uint8_t *get_target(CPU *cpu, Target target);
void execute_opcode(CPU *cpu, Instruction opcode, Target target, int bit_index, int address);

#endif	// CPU_H
