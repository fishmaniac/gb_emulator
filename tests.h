#ifdef TESTS
#ifndef TESTS_H
#define TESTS_H

#include "cpu.h"
#include <stdbool.h>
#include <stdint.h>

char *strrev(char *s);
char *print_binary(uint16_t val);

void test_log(bool test, char *func);

void run_print_binary_tests();

void run_reg_target_tests();

void run_register_set_get();
void run_register_flag_permutations();
void run_register_flag();
void run_register_tests();

// void run_instruction_ld();
void run_instruction_adc();
void run_instruction_add();
void run_instruction_addhl();
void run_instruction_and();
void run_instruction_andhl();
void run_instruction_bit();
void run_instruction_call();
void run_instruction_di();
void run_instruction_ei();
void run_instruction_tests();

void run_stack_tests();

void run_memory_tests();

void run_rom_tests();

void run_all_tests();

#endif	// TESTS
#endif	// TESTS_H
