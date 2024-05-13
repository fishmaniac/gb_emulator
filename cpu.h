#include <stdint.h>
#include <stdbool.h>

typedef struct FlagRegister_t {
	bool zero;
	bool subtract;
	bool half_carry;
	bool carry;
} FlagRegister;

uint8_t flag_to_byte(FlagRegister *flags);

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
