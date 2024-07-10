#ifdef TESTS
#ifndef TESTS_H
#define TESTS_H

#include <stdbool.h>
#include <stdint.h>

char *print_binary(uint16_t val);
void test_log(bool test, char *func);
void run_register_set_get();
void run_flag_register_permutations();
void run_register_flag();
void run_register_tests();
void run_print_binary_tests();
void run_opcode_adc();
void run_opcode_add();
void run_opcode_addhl();
void run_opcode_and();
void run_opcode_bit();
void run_opcode_call();
void run_opcode_di();
void run_opcode_ei();
void run_opcode_tests();
void run_all_tests();

#endif	// TESTS
#endif	// TESTS_H
