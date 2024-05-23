#ifdef TESTS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"

char *strrev(char *s) {
	int len = strlen(s);
	for (int i = 0; i < len / 2; i++) {
		char temp = s[i];
		s[i] = s[len - i - 1];
		s[len - i - 1] = temp;
	}
	return s;
}

char *print_binary(uint16_t val) {
	int temp = val;
	int r = floor(log2(val) + 1);

	char *num = (char *) malloc(r * sizeof(char));
	if (r == 0) {
		return "0";
	}

	for (int i = 0; i < r; i++) {
		int n = temp % 2;
		temp /= 2;

		num[i] = n + '0';
	}
	num[r] = '\0';
	return strrev(num);
}

void test_log(bool test, char *func) {
	if (test) {
		printf("TEST\t%s\tPASSED --- ✅\n", func);
	}
	else {
		printf("TEST\t%s\tFAILED --- ❌\n", func);
	}
}

void run_register_set_get() {
	Register testReg;
	int v;
	for (int i = 0; i <= 65535; i += 4369) {
		set_reg_bc(&testReg, i);
		v = get_reg_bc(&testReg);
		test_log(
			v == i,
			"get_reg_bc"
		);
		set_reg_de(&testReg, i);
		v = get_reg_de(&testReg);
		test_log(
			v == i,
			"get_reg_de"
		);
		set_reg_hl(&testReg, i);
		v = get_reg_hl(&testReg);
		test_log(
			v == i,
			"get_reg_hl"
		);
	}
}

void run_register_flag() {
	Register testReg;

	for (int i = 0; i <= 15; i++) {
		set_reg_f(&testReg.f, i << 4);
		uint8_t v = get_reg_f(&testReg.f);
		test_log(
			v == i << 4,
			"get_reg_f"
		);
	}
	int j = 0;
	for (int i = 0; i <= 255; i += 17, j++) {
		uint16_t num = (i << 8) | (j << 4);
		set_reg_af(&testReg, num);
		uint16_t v = get_reg_af(&testReg);
		test_log(
			v == num,
			"get_reg_af"
		);
	}
}

void run_register_tests() {
	run_register_set_get();
	run_register_flag();
}

void run_print_binary_tests() {
	char *v = print_binary(15);
	test_log(
		strcmp(v, "1111") == 0,
		"print_binary(15)"
	);

	free(v);

	v = print_binary(543);
	test_log(
		strcmp(v, "1000011111") == 0,
		"print_binary(543)"
	);

	free(v);

	v = print_binary(9453);
	test_log(
		strcmp(v, "10010011101101") == 0,
		"print_binary(9453)"
	);

	free(v);

	v = print_binary(18968);
	test_log(
		strcmp(v, "100101000011000") == 0,
		"print_binary(18968)"
	);

	free(v);

	v = print_binary(65535);
	test_log(
		strcmp(v, "1111111111111111") == 0,
		"print_binary(65535)"
	);

	free(v);
}

// Test 8 bit add to A register
void run_opcode_add() {
	CPU cpu;
	bool pass;

	/*	here we run out of bits,
	 *	an overflow occurs, 
	 *	so carry is set
	 *	  \
	 *	  1111
	 *	0b11110000 == 240
	 *	0b00011110 == 30
	 *     +__________
	 *     0b100001110 -> 0b00001110
	 *	 ^ carry bit excluded, so the result
	 *	 is 0b1110
	 *
	 *	 240 + 30 == 270
	 *	 270 - 255 - 1 == 14
	 *	 * carry
	 */
	cpu.reg.a = 0b11110000;
	cpu.reg.b = 0b00011110;

	execute_opcode(&cpu, ADD, B);

	pass = cpu.reg.a == 0b00001110
		&& cpu.reg.f.zero == false
		&& cpu.reg.f.subtract == false
		&& cpu.reg.f.carry == true
		&& cpu.reg.f.half_carry == false;
	test_log(
		pass,
		"ADD --- 240 + 30 == 14"
	);

	/*	0b10001111 == 143
	 *	0b00000001 == 1
	 *     +__________
	 *	0b10010000
	 *	     ^ here 4 bit barrier is crossed (0xF/UINT4_MAX),
	 *	       meaning we overflowed the nibble (4 bits),
	 *	       so half carry is set
	 *
	 *	143 + 1 == 144
	 *	* half carry
	 */
	cpu.reg.a = 0b10001111;
	cpu.reg.b = 0b00000001;

	execute_opcode(&cpu, ADD, B);

	pass = cpu.reg.a == 0b10010000
		&& cpu.reg.f.zero == false
		&& cpu.reg.f.subtract == false
		&& cpu.reg.f.carry == false
		&& cpu.reg.f.half_carry == true;
	test_log(
		pass,
		"ADD --- 143 + 1 == 144"
	);
}
void run_opcode_addhl() {
	CPU cpu;
	bool pass;

	execute_opcode(&cpu, ADDHL, B);

	pass = false;

	test_log(
		pass,
		"ADDHL --- "
	);
}

// Test 8 bit add to A register with carry
void run_opcode_adc() {
	CPU cpu;
	bool pass;

	/*	here we run out of bits,
	 *	an overflow occurs, 
	 *	so carry is set
	 *	  \
	 *	  1111
	 *	0b11110000 == 240
	 *	0b00011110 == 30
	 *     +__________
	 *     0b100001110 -> 0b00001111
	 *	 ^ carry bit included, so we carry it
	 *	 to the LSB, and the result is 0b1111
	 *
	 *	 240 + 30 == 270
	 *	 270 - 255 == 14
	 *	 * carry
	 */	
	cpu.reg.a = 0b11110000;
	cpu.reg.b = 0b00011110;

	execute_opcode(&cpu, ADC, B);

	pass = cpu.reg.a == 15
		&& cpu.reg.f.zero == false
		&& cpu.reg.f.subtract == false
		&& cpu.reg.f.carry == true
		&& cpu.reg.f.half_carry == false;

	test_log(
		pass,
		"ADC --- 240 + 30 + c == 15"
	);

	/*	0b10001111 == 143
	 *	0b00000001 == 1
	 *     +__________
	 *	0b10010000
	 *	     ^ here 4 bit barrier is crossed (0xF/UINT4_MAX),
	 *	       meaning we overflowed the nibble (4 bits),
	 *	       so half carry is set
	 *
	 *	143 + 1 == 144
	 *	* half carry
	 */
	cpu.reg.a = 0b10001111;
	cpu.reg.b = 0b00000001;
	execute_opcode(&cpu, ADC, B);
	pass = cpu.reg.a == 0b10010000
		&& cpu.reg.f.zero == false
		&& cpu.reg.f.subtract == false
		&& cpu.reg.f.carry == false
		&& cpu.reg.f.half_carry == true;
	test_log(
		pass,
		"ADC --- 143 + 1 == 144"
	);
}

void run_opcode_tests() {
	run_opcode_add();
	run_opcode_adc();
}

void run_all_tests() {
	printf("TESTS BEGIN: %d\n", TESTS);
	printf("Register Tests:\n");
	run_register_tests();
	printf("Print Binary Tests:\n");
	run_print_binary_tests();
	printf("Opcode Tests:\n");
	run_opcode_tests();
}

#endif
