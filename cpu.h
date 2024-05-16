#include <stdint.h>
#include <stdbool.h>

#define UINT4_MAX 15 // 0xF
#define UINT8_MAX 255 // 0xFF
#define UINT16_MAX 65535 // 0xFFFF

typedef struct FlagRegister_t {
	bool zero;
	bool subtract;
	bool half_carry;
	bool carry;
} FlagRegister;

uint8_t get_reg_F(FlagRegister *flags);
void set_reg_F(FlagRegister *flags, uint8_t val);

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

} Register;

uint16_t get_reg_AF(Register *reg);
void set_reg_AF(Register *reg, uint16_t val);

uint16_t get_reg_BC(Register *reg);
void set_reg_BC(Register *reg, uint16_t val);

uint16_t get_reg_DE(Register *reg);
void set_reg_DE(Register *reg, uint16_t val);

uint16_t get_reg_HL(Register *reg);
void set_reg_HL(Register *reg, uint16_t val);

uint16_t get_reg_DF(Register *reg);
void set_reg_DF(Register *reg, uint16_t val);

typedef enum {
	ADD,
	ADDHL,
	ADC,
} Opcode;

typedef enum {
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	L
} Target;

typedef struct CPU_t {
	Register reg;
} CPU;


uint8_t add(CPU *cpu, uint8_t val);
uint16_t addhl(CPU *cpu, uint8_t val);

uint8_t *get_target(CPU *cpu, Target target);
void execute_opcode(CPU *cpu, Opcode opcode, Target target);
