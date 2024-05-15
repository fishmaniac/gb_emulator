#ifdef TESTS
#include <stdio.h>
#include <stdbool.h>
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
		set_reg_BC(&testReg, i);
		v = get_reg_BC(&testReg);
		test_log(
			v == i,
			"get_reg_BC"
		);
		set_reg_DE(&testReg, i);
		v = get_reg_DE(&testReg);
		test_log(
			v == i,
			"get_reg_DE"
		);
		set_reg_HL(&testReg, i);
		v = get_reg_HL(&testReg);
		test_log(
			v == i,
			"get_reg_HL"
		);
	}
}

void run_register_flag() {
	Register testReg;

	for (int i = 0; i <= 15; i++) {
		set_reg_F(&testReg.f, i << 4);
		uint8_t v = get_reg_F(&testReg.f);
		test_log(
			v == i << 4,
			"get_reg_F"
		);
	}
	int j = 0;
	for (int i = 0; i <= 255; i += 17, j++) {
		uint16_t num = (i << 8) | (j << 4);
		set_reg_AF(&testReg, num);
		uint16_t v = get_reg_AF(&testReg);
		test_log(
			v == num,
			"get_reg_AF"
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

void run_all_tests() {
	printf("TESTS BEGIN: %d\n", TESTS);
	printf("Register Tests:\n");
	run_register_tests();
	run_print_binary_tests();
}

#endif
