#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

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

char *print_binary(int val) {
	printf("reg: %d\t", val);

	int temp = val;
	int r = floor(log2(val) + 1);
	char *num = (char *) malloc(r * sizeof(char));

	// just free this manually
	addMalloc(num);

	for (int i = 0; i < r; i++) {
		int n = temp % 2;
		temp /= 2;

		printf("i: %d, n: %d\n", i, n);
		num[i] = n + '0';
	}

	num[r] = '\0';


	printf("binary: %s\n", num);
	return num;
}


#ifdef TESTS

#define FUNCTION_NAME __func__
void test_log(bool test, char *func) {
	if (test) {
		printf("TEST\t%s\tPASSED --- ✅\n", func);
	}
	else {
		printf("TEST\t%s\tFAILED --- ❌\n", func);
	}
}

void run_all_tests() {
	printf("TESTS BEGIN: %d\n", TESTS);
	printf("\nPRINT BINARY TESTS BEGIN\n\n");
	char* n = print_binary(60234);
	printf("printbin: %s\n", n);
	test_log(1, FUNCTION_NAME);

	test_log(1, FUNCTION_NAME);
	test_log(0, FUNCTION_NAME);

}
#endif

int main(int argc, char *argv[]) {
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
	#ifdef TESTS
run_all_tests();
	#endif
	freeMalloc();

	return 0;
}
