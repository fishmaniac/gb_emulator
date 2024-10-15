#include <SDL2/SDL_timer.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "cpu.h"
#include "constants.h"
#include "memory.h"
#include "timer.h"
#include "window.h"

#include <SDL2/SDL.h>

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

	CPU cpu;
	init_cpu(&cpu);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	int window_x = 160;
	int window_y = 144;
	SDL_Window *window = SDL_CreateWindow("Gameboy", SDL_WINDOWPOS_UNDEFINED,
				       SDL_WINDOWPOS_UNDEFINED, window_x, window_y, 0);
	if (!window) {
		printf("Failed to open %d x %d window: %s\n", window_x, window_y, SDL_GetError());
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		printf("Failed to create renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
					  SDL_TEXTUREACCESS_STREAMING, window_x, window_y);
	if (!texture) {
		printf("Failed to create texture: %s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Delay(500);

	read_rom(&cpu.map, "asm/tetris.gb");
	load_rom(&cpu.map, 0);
	// cpu.reg.a = 0;
	// cpu.reg.b = 5;
	// cpu.reg.h = 3;
	// cpu.map.memory[0x100] = 0x80; // ADD A, B = 5
	// cpu.map.memory[0x101] = 0x84; // ADD A, H = 8
	// cpu.map.memory[0x102] = 0x88; // ADC A, B = 13
	// cpu.map.memory[0x103] = 0x90; // SUB A, B = 8
	// cpu.map.memory[0x104] = 0xC0;
	// cpu.map.memory[0x105] = 0x76; // HALT
	// write_memory(&cpu.map, TAC_ADDR, 0b00000100);

	int execute_cycles = 6;
	int div_cycle_count = 0;
	int tima_cycle_count = 0;

	static const double FPS = 120;
	static const double TARGET_FRAME_TIME = 1000.0 / FPS;

	static const int CYCLES_PER_FRAME = CPU_FREQ / FPS;
	int cycles_this_frame = 0;

	while (!cpu.halt) {
		get_input(&cpu);

		uint64_t frame_start_time = SDL_GetPerformanceCounter();

		if (cpu.ime_next) {
			cpu.ime = true;
			cpu.ime_next = false;
		}
		handle_interrupt(&cpu);
		execute_cycles = execute_instruction(&cpu);
		execute_cycles = 6;
		div_cycle_count += execute_cycles;
		tima_cycle_count += execute_cycles;

		if (div_cycle_count >= DIV_CYCLE_INTERVAL) {
			cpu.map.memory[DIV_ADDR]++;
			div_cycle_count -= DIV_CYCLE_INTERVAL;
		}

		int tima_interval = CPU_FREQ / get_tac_clock(&cpu.map);
		if (tima_cycle_count >= tima_interval) {
			increment_tima(&cpu.map);
			tima_cycle_count -= tima_interval;
		}
		if (cycles_this_frame >= CYCLES_PER_FRAME) {
			request_interrupt(&cpu.map, VBlank);
			break;
		}
		update_tile_data_texture(&cpu, window, renderer, texture);
		render(renderer, texture);

		uint64_t frame_end_time = SDL_GetPerformanceCounter();
		double elapsed_time = (double)(frame_end_time - frame_start_time) / SDL_GetPerformanceFrequency() * 1000.0;

		if (elapsed_time < TARGET_FRAME_TIME) {
			// SDL_Delay((uint32_t)(TARGET_FRAME_TIME - elapsed_time));
		}
	}


	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
