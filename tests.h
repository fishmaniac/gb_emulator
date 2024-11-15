#ifdef TESTS
#ifndef TESTS_H
#define TESTS_H

#include <stdbool.h>
#include <stdint.h>

char *strrev(char *s);
char *print_binary(uint16_t val);

void test_log(bool test, char *func);

void run_print_binary_tests(void);

void run_reg_target_tests(void);

void run_register_set_get(void);
void run_register_flag_permutations(void);
void run_register_flag(void);
void run_register_tests(void);

// void run_instruction_ld(void);
void run_instruction_adc(void);
void run_instruction_add(void);
void run_instruction_addhl(void);
void run_instruction_and(void);
void run_instruction_andhl(void);
void run_instruction_bit(void);
void run_instruction_call(void);
void run_instruction_di(void);
void run_instruction_ei(void);
void run_instruction_tests(void);

void run_stack_tests(void);

void run_memory_tests(void);

void run_rom_tests(void);

void run_all_tests(void);

#endif	// TESTS
#endif	// TESTS_H
