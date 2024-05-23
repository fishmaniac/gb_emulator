#include <stdint.h>
#include <stdbool.h>

#define UINT4_MAX 15		// 0xF
#undef UINT8_MAX
#define UINT8_MAX 255		// 0xFF
#define UINT12_MAX 4095		// 0xFFF
#undef UINT16_MAX
#define UINT16_MAX 65535	// 0xFFFF


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

} Register;

uint16_t get_reg_af(Register *reg);
void set_reg_af(Register *reg, uint16_t val);

uint16_t get_reg_bc(Register *reg);
void set_reg_bc(Register *reg, uint16_t val);

uint16_t get_reg_de(Register *reg);
void set_reg_de(Register *reg, uint16_t val);

uint16_t get_reg_hl(Register *reg);
void set_reg_hl(Register *reg, uint16_t val);

uint16_t get_reg_df(Register *reg);
void set_reg_df(Register *reg, uint16_t val);

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
	L,
} Target;

typedef struct CPU_t {
	Register reg;
} CPU;


uint8_t add(CPU *cpu, uint8_t val);
uint16_t addhl(CPU *cpu, uint8_t val);

uint8_t *get_target(CPU *cpu, Target target);
void execute_opcode(CPU *cpu, Opcode opcode, Target target);
