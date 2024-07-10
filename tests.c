#include "memory.h"
#ifdef TESTS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "tests.h"

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
	int func_len_max = 50;
	int func_len = strlen(func);
	char* result = test 
		? "TEST\t%s%sPASSED --- ✅" 
		: "TEST\t%s%sFAILED --- ❌";

	int offset_len;
	if (func_len > func_len_max) {
		printf("test_log func is above max length\n");
		// Set minimum offset to 1
		offset_len = 1;
	}
	else {
		offset_len = func_len_max - func_len;
	}
	// 23 for `result` string,
	// offset keeps all strings the same size,
	// +1 for \0
	int str_len = 23 + func_len + offset_len + 1;

	char *str = malloc(str_len);
	char offset_str[offset_len];

	for (int i = 0; i < offset_len; i++) {
		offset_str[i] = ' ';
	}
	offset_str[offset_len] = '\0';

	snprintf(str, str_len, result, func, offset_str);

	printf("%s\n", str);

	free(str);
}
void run_register_set_get() {
	Register test_reg;
	int v;
	int count = 0;
	int expect = 327680; // (65535 + 1) * 5
	for (int i = 0; i <= 65535; i++) {
		set_reg_bc(&test_reg, i);
		v = get_reg_bc(&test_reg);
		if (v == i) count++;

		set_reg_de(&test_reg, i);
		v = get_reg_de(&test_reg);
		if (v == i) count++;

		set_reg_hl(&test_reg, i);
		v = get_reg_hl(&test_reg);
		if (v == i) count++;

		set_reg_sp(&test_reg, i);
		v = get_reg_sp(&test_reg);
		if (v == i) count++;

		set_reg_pc(&test_reg, i);
		v = get_reg_pc(&test_reg);
		if (v == i) count++;
	}
	test_log(
		count == expect,
		"set_get_16_bit_reg"
	);
}
void run_flag_register_permutations() {
	Register test_reg;
	int count = 0;
	int expect = 16; // 15 + 1

	for (int i = 0; i <= 15; i++) {
		set_reg_f(&test_reg.f, i << 4);
		uint8_t v = get_reg_f(&test_reg.f);
		if (v == i << 4) count++;
	}
	test_log(
		count == expect,
		"flag_register_permutations"
	);
}
void run_register_flag() {
	Register test_reg;
	int count = 0;
	int expect = 16;

	for (int i = 0, j = 0; i <= 255; i += 17, j++) {
		uint16_t num = (i << 8) | (j << 4);
		set_reg_af(&test_reg, num);
		uint16_t v = get_reg_af(&test_reg);

		if (v == num) count++;

	}
	test_log(
		count == expect,
		"get_reg_af"
	);
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

	execute_opcode(&cpu, ADC, B, 0, 0);

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
	execute_opcode(&cpu, ADC, B, 0, 0);
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

	execute_opcode(&cpu, ADD, B, 0, 0);

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

	execute_opcode(&cpu, ADD, B, 0, 0);

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

	execute_opcode(&cpu, ADDHL, B, 0, 0);

	pass = false;

	test_log(
		pass,
		"ADDHL --- "
	);
}
void run_opcode_addsp() {
	CPU cpu;
	bool pass;

	execute_opcode(&cpu, ADDSP, B, 0, 0);

	pass = false;

	test_log(
		pass,
		"ADDSP --- "
	);
}
void run_opcode_and() {
	CPU cpu;
	bool pass;

	execute_opcode(&cpu, AND, B, 3, 1);

	pass = false;

	test_log(
		pass,
		"AND --- "
	);
}
void run_opcode_bit() {
	CPU cpu;
	bool pass;

	execute_opcode(&cpu, BIT, A, 5, 0);

	pass = false;

	test_log(
		pass,
		"BIT --- "
	);
}
void run_opcode_call() {
	CPU cpu;
	bool pass;

	execute_opcode(&cpu, CALL, NONE, 0, 0xFFF);

	pass = false;

	test_log(
		pass,
		"CALL --- "
	);
}
void run_opcode_di() {
	CPU cpu;
	bool pass;

	execute_opcode(&cpu, DI, NONE, 0, 0xFFF);

	pass = false;

	test_log(
		pass,
		"DI --- "
	);
}
void run_opcode_ei() {
	CPU cpu;
	bool pass;

	execute_opcode(&cpu, EI, NONE, 0, 0xFFF);

	pass = false;

	test_log(
		pass,
		"EI --- "
	);
}
void run_opcode_tests() {
	run_opcode_add();
	run_opcode_adc();
	run_opcode_addhl();
	run_opcode_addsp();
	run_opcode_and();
	run_opcode_bit();
	run_opcode_call();
	run_opcode_di();
	run_opcode_ei();
}
void run_memory_tests() {
	CPU cpu;
	int count = 0;
	int count_expect = MEMORY_SIZE;
	bool pass;

	cpu.reg.a = 1;
	for (int i = 0; i < MEMORY_SIZE; i++) {
		uint8_t expect = cpu.reg.a + cpu.reg.a;

		execute_opcode(&cpu, ADD, A, -1, -1);
		write_memory(&cpu.map, i, cpu.reg.a);
		pass = read_memory(&cpu.map, i) == expect;

		if (pass) count++;
	}

	// + 11 to allow space for "memory --- ", + 13 for " passed cases", + 1 for \0
	int str_len = 11 + snprintf(NULL, 0, "%d", count) + 13 + 1;
	char* str = malloc(str_len);
	snprintf(str, str_len, "memory --- %d passed cases", count);

	test_log(
		count == count_expect,
		str
	);
	free(str);
}
void run_all_tests() {
	printf("-----------------------------------------------------------------------------------------------------------------------\n");
	printf("TESTS BEGIN: %d\n", TESTS);
	printf("Register Tests:\n");
	run_register_tests();
	printf("Print Binary Tests:\n");
	run_print_binary_tests();
	printf("Opcode Tests:\n");
	run_opcode_tests();
	printf("Memory Tests:\n");
	run_memory_tests();
}

#endif
