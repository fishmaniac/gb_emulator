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

	// cpu.reg.pc = 0x0;
	// while (cpu.reg.pc < 0x100) {
	// 	uint8_t opcode = cpu.map.memory[cpu.reg.pc++];
	// 	// TODO: cpu.execute_opcode(opcode);
	// }

	return 0;
}
