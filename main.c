#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cpu.h"
#ifdef TESTS
#include "tests.h"

void init(void)  __attribute__((constructor));
void init(void) {
	run_all_tests();
}
#endif

#define RAM_SIZE 512

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
	printf("main\n");

	CPU cpu;
	// Should carry
	cpu.reg.a = 240;
	cpu.reg.b = 30;
	// Should half carry
	// cpu.reg.a = 0b10001111;
	// cpu.reg.b = 1;
	execute_opcode(&cpu, ADD, B);
	printf(
		"A: %d\tZero: %d\tSubtract: %d\tCarry: %d\tHalf Carry: %d\n",
		cpu.reg.a,
		cpu.reg.f.zero,
		cpu.reg.f.subtract,
		cpu.reg.f.carry,
		cpu.reg.f.half_carry
	);

	return 0;
}
