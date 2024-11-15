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
	if (val == 0) {
		// Malloc to free always
		char *num = (char *) malloc(2 * sizeof(char));
		num[0] = '0';
		num[1] = '\0';

		return num;
	}

	int temp = val;
	int bits = floor(log2(val) + 1);
	char *num = (char *) malloc(bits * sizeof(char));

	for (int i = 0; i < bits; i++) {
		int n = temp % 2;
		temp /= 2;

		num[i] = n + '0';
	}

	num[bits] = '\0';
	return strrev(num);
}
void test_log(bool test, char *func) {
	const int func_len_max = 50;
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
void run_print_binary_tests(void) {
	char *v = print_binary(0);
	test_log(
		strcmp(v, "0") == 0,
		"print_binary(0)"
	);

	free(v);

	v = print_binary(15);
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
// 16 bit reg is stored as little endian
void run_reg_target_tests(void) {
	{
		CPU cpu;
		uint16_t *_16 = get_target_reg_16(&cpu, BC);

		*_16 = 0b1010111111100111;

		uint8_t b = cpu.reg.b;
		uint8_t c = cpu.reg.c;

		bool pass = b == 0b10101111
			&& c == 0b11100111;

		test_log(
			pass,
			"Target Test --- bc"
		);
	}
	{
		CPU cpu;
		uint16_t *_16 = get_target_reg_16(&cpu, DE);

		*_16 = 0b1010111111100111;

		uint8_t d = cpu.reg.d;
		uint8_t e = cpu.reg.e;

		bool pass = d == 0b10101111
			&& e == 0b11100111;

		test_log(
			pass,
			"Target Test --- de"
		);
	}
	{
		CPU cpu;
		uint16_t *_16 = get_target_reg_16(&cpu, HL);

		*_16 = 0b1010111111100111;

		uint8_t h = cpu.reg.h;
		uint8_t l = cpu.reg.l;

		bool pass = h == 0b10101111
			&& l == 0b11100111;

		test_log(
			pass,
			"Reg Target Test --- hl"
		);
	}
}

void run_register_set_get(void) {
	// Register test_reg;
	// int v;
	// int count = 0;
	// int expect = 327675; // (65535 + 1) * 5
	// for (int i = 0; i < 65535; i++) {
	// 	set_reg_bc(&test_reg, i);
	// 	v = get_reg_bc(&test_reg);
	// 	if (v == i) count++;
	//
	// 	set_reg_de(&test_reg, i);
	// 	v = get_reg_de(&test_reg);
	// 	if (v == i) count++;
	//
	// 	set_reg_hl(&test_reg, i);
	// 	v = get_reg_hl(&test_reg);
	// 	if (v == i) count++;
	// }
	// test_log(
	// 	count == expect,
	// 	"set_get_16"
	// );
	CPU cpu;
	init_cpu(&cpu);

	set_reg_af(&cpu.reg, 0x1234);
	test_log(
		get_reg_af(&cpu.reg) == 0x1234,
		"set_get_af"
	);

	set_reg_bc(&cpu.reg, 0x1234);
	test_log(
		get_reg_bc(&cpu.reg) == 0x1234,
		"set_get_bc"
	);

	set_reg_de(&cpu.reg, 0x1234);
	test_log(
		get_reg_de(&cpu.reg) == 0x1234,
		"set_get_de"
	);

	set_reg_hl(&cpu.reg, 0x1234);
	test_log(
		get_reg_hl(&cpu.reg) == 0x1234,
		"set_get_hl"
	);



}
void run_register_flag_permutations(void) {
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
void run_register_flag(void) {
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
void run_register_tests(void) {
	run_register_set_get();
	run_register_flag();
	run_register_flag_permutations();
}
// Test load
// void run_instruction_ld(void) {
// 	CPU cpu;
// 	bool pass;
// 	int expect = 15;
//
// 	ld_r8_n8(&cpu, B, expect);
// 	int val = cpu.reg.b;
// 	pass = val == expect;
// 	test_log(
// 		pass,
// 		"LD --- B, 15"
// 	);
//
// 	ld_r8_r8(&cpu, H, B);
// 	val = cpu.reg.h;
// 	pass = val == expect;
// 	test_log(
// 		pass,
// 		"LD --- H, B"
// 	);
//
// 	// Store B at addr
// 	uint16_t addr = 2048;
// 	set_reg_hl(&cpu.reg, addr);
// 	ld_hl_n8(&cpu, cpu.reg.b);
// 	uint8_t mem = read_memory(&cpu.map, addr);
// 	pass = mem == expect;
// 	test_log(
// 		pass,
// 		"LD --- HL, B"
// 	);
//
// 	// Load addr into D
// 	printf("mem: %d\thl: %d\n", mem, get_reg_hl(&cpu.reg));
// 	ld_r8_hl(&cpu, D);
// 	printf("prev val: %d\taddr: %d\n", val, addr);
// 	val = cpu.reg.d;
// 	printf("new val: %d\texpect: %d\n", val, expect);
// 	pass = val == expect;
// 	test_log(
// 		pass,
// 		"LD --- D, HL"
// 	);
//
// 	set_reg_hl(&cpu.reg, 4096);
// 	// Load D into HL
// 	ld_hl_r8(&cpu, D);
// 	val = read_memory(&cpu.map, 4096);
// 	pass = val == expect;
// 	test_log(
// 		pass,
// 		"LD --- HL, D"
// 	);
// }
// Test 8 bit add to A register with carry
void run_instruction_adc(void) {
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
	uint8_t *_8 = get_target_reg_8(&cpu, B);
	adc(&cpu, *_8);

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
	cpu.reg.f.carry = false;
	cpu.reg.a = 0b10001111;
	cpu.reg.b = 0b00000001;
	printf("before reg a: %d, reg b: %d\n", cpu.reg.a, cpu.reg.b);
	adc(&cpu, cpu.reg.b);
	printf("after reg a: %d, reg b: %d\n", cpu.reg.a, cpu.reg.b);

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
void run_instruction_add(void) {
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
	add_r8(&cpu, B);

	pass = cpu.reg.a == 0b00001110
		&& cpu.reg.f.zero == false
		&& cpu.reg.f.subtract == false
		&& cpu.reg.f.carry == true
		&& cpu.reg.f.half_carry == false;
	test_log(
		pass,
		"ADD --- 240 + 30 == 14 (overflow)"
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

	add_r8(&cpu, B);

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
void run_instruction_addhl(void) {
	CPU cpu;
	bool pass;

	addhl(&cpu, cpu.reg.b);

	pass = false;

	test_log(
		pass,
		"ADDHL --- "
	);
}
void run_instruction_addsp(void) {
	CPU cpu;
	bool pass;

	pass = false;

	test_log(
		pass,
		"ADDSP --- "
	);
}
void run_instruction_and(void) {
	CPU cpu;
	bool pass;

	pass = false;

	test_log(
		pass,
		"AND --- "
	);
}
void run_instruction_andhl(void) {
	CPU cpu;
	bool pass;

	pass = false;

	test_log(
		pass,
		"ANDHL --- "
	);
}
void run_instruction_bit(void) {
	CPU cpu;
	bool pass;

	pass = false;

	test_log(
		pass,
		"BIT --- "
	);
}
void run_instruction_call(void) {
	CPU cpu;
	bool pass;

	pass = false;

	test_log(
		pass,
		"CALL --- "
	);
}
void run_instruction_di(void) {
	CPU cpu;
	bool pass;

	pass = false;

	test_log(
		pass,
		"DI --- "
	);
}
void run_instruction_ei(void) {
	CPU cpu;
	bool pass;

	pass = false;

	test_log(
		pass,
		"EI --- "
	);
}
void run_instruction_tests(void) {
	// run_instruction_ld();
	run_instruction_add();
	run_instruction_adc();
	run_instruction_addhl();
	run_instruction_addsp();
	run_instruction_and();
	run_instruction_andhl();
	run_instruction_bit();
	run_instruction_call();
	run_instruction_di();
	run_instruction_ei();
}
void run_memory_tests(void) {
	CPU cpu;
	int count = 0;
	int count_expect = MEMORY_SIZE;
	bool pass;

	cpu.reg.a = 1;
	for (int i = 0; i < MEMORY_SIZE; i++) {
		uint8_t expect = cpu.reg.a + cpu.reg.a;

		add_r8(&cpu, A);
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
void run_stack_tests(void) {
	{	
		CPU cpu;
		init_cpu(&cpu);
		set_sp_addr(&cpu, 255);

		uint16_t expect = 0x1234;
		set_reg_de(&cpu.reg, expect);

		push_r16(&cpu, DE);
		pop_r16(&cpu, BC);

		uint16_t val = get_reg_bc(&cpu.reg);

		test_log(
			val == expect,
			"Stack DE"
		);
	}
	{	
		CPU cpu;
		init_cpu(&cpu);
		set_sp_addr(&cpu, 0x100);

		int expect = 0x1234 + 0xAF20 + 0x9843 + 0x9292 + 0x0003 + 0xCB50 + 0x3331 + 0x00B4;
		int sum = 0;

		set_reg_bc(&cpu.reg, 0x1234);
		push_r16(&cpu, BC);
		set_reg_de(&cpu.reg, 0xAF20);
		push_r16(&cpu, DE);
		set_reg_hl(&cpu.reg, 0x9843);
		push_r16(&cpu, HL);
		set_reg_bc(&cpu.reg, 0x9292);
		push_r16(&cpu, BC);
		set_reg_de(&cpu.reg, 0x0003);
		push_r16(&cpu, DE);
		set_reg_af(&cpu.reg, 0xCB50);
		push_n16(&cpu, get_reg_af(&cpu.reg));
		sum += pop_n16(&cpu);

		set_reg_bc(&cpu.reg, 0x3331);
		push_r16(&cpu, BC);
		pop_r16(&cpu, HL);
		sum += get_reg_hl(&cpu.reg);

		set_reg_hl(&cpu.reg, 0x00B4);
		push_r16(&cpu, HL);
		// set_reg_af(&cpu.reg, 0xAF3F);
		// push(&cpu, AF);

		// printf("reg af: 0x%x\n", get_reg_af(&cpu.reg));

		for (int i = 0; i < 6; i++) {
			pop_r16(&cpu, BC);
			sum += get_reg_bc(&cpu.reg);
		}

		test_log(
			sum == expect,
			"Stack BC, DE, HL, AF"
		);
	}

}
void run_rom_tests(void) {

}
void run_all_tests(void) {
	printf("-----------------------------------------------------------------------------------------------------------------------\n");
	printf("TESTS BEGIN: %d\n", TESTS);
	printf("Print Binary Tests:\n");
	run_print_binary_tests();
	printf("Reg Target Tests:\n");
	run_reg_target_tests();
	printf("Register Tests:\n");
	run_register_tests();
	printf("Instruction Tests:\n");
	run_instruction_tests();
	printf("Memory Tests:\n");
	run_memory_tests();
	printf("Stack Tests:\n");
	run_stack_tests();
	printf("ROM Tests:\n");
	run_rom_tests();
}

#endif
