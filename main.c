#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "cpu.h"
#include "constants.h"
#include "memory.h"

#ifdef TESTS
#include "tests.h"

void init(void)  __attribute__((constructor));
void init(void) {
	run_all_tests();
}
#endif

typedef struct MallocMap_t {
	void *var;
	struct MallocMap_t *next;
} MallocMap;

MallocMap *mallocMap = NULL;

void addMalloc(void *var) {
	MallocMap *node = (MallocMap *) malloc(sizeof(MallocMap));

	node->var = var;
	node->next = NULL;

	if (mallocMap == NULL) {
		mallocMap = node;
	}
	else {
		MallocMap *current = mallocMap;

		while (current->next != NULL) {
			current = current->next;
		}

		current->next = node;
	}
}

void freeMalloc() {
	MallocMap *node = mallocMap;
	MallocMap *next;

	while (node != NULL) {
		next = node->next;
		free(node->var);
		free(node);
		node = next;
	}

	free(mallocMap);
	mallocMap = NULL;
}

int main(int argc, char *argv[]) {
	printf("-----------------------------------------------------------------------------------------------------------------------\n");
	printf("main\targc: %d\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("i: %d, argv[i]: %s\n", i, argv[i]);
	}

	// TODO: Load rom
	CPU cpu;
	init_cpu(&cpu);
	
	load_simple_rom(&cpu.map);
	// cpu.reg.a = 0;
	// cpu.reg.b = 5;
	// cpu.reg.h = 3;
	// cpu.map.memory[0x100] = 0x80; // ADD A, B = 5
	// cpu.map.memory[0x101] = 0x84; // ADD A, H = 8
	// cpu.map.memory[0x102] = 0x88; // ADC A, B = 13
	// cpu.map.memory[0x103] = 0x90; // SUB A, B = 8
	// cpu.map.memory[0x104] = 0xC0;
	cpu.map.memory[0x105] = 0x76; // HALT

	uint8_t opcode;
	// TODO: should halt until interrupt
	// TODO: use switch... or no execute
	while(!cpu.halt) {
		opcode = cpu.map.memory[cpu.pc++];
		printf("start pc: %d, opcode: 0x%x, a: %d, mem: %d\n", cpu.pc, opcode, cpu.reg.a, cpu.map.memory[cpu.pc]);
		printf("test: 0x%x - 0x%x\n", opcode & 0x7, (opcode >> 3 & 0x7));
		// NOP
		if (opcode >= 0x00 && opcode < 0x40) {
			switch (opcode) {
				// NOP
				case 0x00:
					printf("Executed NOP\n");
					break;
				// STOP
				case 0x10:
					cpu.halt = true;
					break;
			}
		}
		// Shortcut for load instructions
		else if (opcode >= 0x40 && opcode < 0x80) {
			// HALT
			if (opcode == 0x76) {
				printf("Executing HALT\n");
				cpu.halt = true;
				continue;
			}
			int src = opcode & 0x7;
			int dest = (opcode >> 3) & 0x7;
			// TODO: case for reg HL, HALT
			Target_8 reg_src = decode_register_8(src);
			Target_8 reg_dest = decode_register_8(dest);

			*get_target_reg_8(&cpu, reg_dest) = *get_target_reg_8(&cpu, reg_src);
			printf("Executed LD r8, r8: opcode: 0x%x, acc: %d, src: %d - %d, dest: %d - %d\n\n", cpu.reg.a, opcode, src, *get_target_reg_8(&cpu, reg_src), dest, *get_target_reg_8(&cpu, reg_dest));
		}
		// Math instructions
		// Add, sub, and, or
		else if (opcode >= 0x80 && opcode < 0xC0) {
			int src = opcode & 0x7;
			int operation = (opcode >> 3) & 0x7;
			Target_8 reg_src = decode_register_8(src);

			execute_instruction(&cpu, operation, reg_src, BC, -1, -1);
			printf("Executed MATH: opcode: 0x%x, operation: %d, acc: %d, src: %d - %d\n", opcode, operation, cpu.reg.a, src, *get_target_reg_8(&cpu, reg_src));
		}
		else if (opcode >= 0xC0 && opcode <= 0xFF) {
			printf("Executing STACK\n");
		}
		else {
			printf("[ERROR] - Unknown instruction\n");
		}
		printf("end pc: %d, opcode: 0x%x, a: %d, mem: %d\n", cpu.pc, opcode, cpu.reg.a, cpu.map.memory[cpu.pc]);
	} 
	// cpu.reg.pc = 0x0;
	// while (cpu.reg.pc < 0x100) {
	// 	uint8_t opcode = cpu.map.memory[cpu.reg.pc++];
	// 	// TODO: cpu.execute_opcode(opcode);
	// }

	return 0;
}
