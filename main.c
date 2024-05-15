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
	Register reg;

	set_reg_BC(&reg, 65534);
	/* set_reg_AF(&reg, 0b0000111100001111); */
	reg.a = 0b11111111;
	reg.f.zero = 0;
	reg.f.subtract = 1;
	reg.f.half_carry = 0;
	reg.f.carry = 1;

	// print_binary(get_reg_BC(&reg));
	// print_binary(get_reg_AF(&reg));

	return 0;
}
