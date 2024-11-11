#include "window.h"
#include "cpu.h"
#include "tests.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

void get_input(CPU *cpu) {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			cpu->halt = true;
		}
		else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				cpu->halt = true;
			}
		}
	}
}
void render(SDL_Renderer *renderer, SDL_Texture *texture) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}
void update_tile_texture(CPU *cpu, uint32_t* pixel_ptr, uint16_t addr, int screen_x, int screen_y) {
	for (int y = 0; y < 8; y++) {
		uint8_t left_line = cpu->map.memory[addr + y * 2];
		uint8_t right_line = cpu->map.memory[addr + y * 2 + 1];
		for (int x = 0; x < 8; x++) {
			int bit = 7 - x;
			uint8_t color_id = ((right_line >> bit) & 1) << 1 | ((left_line >> bit) & 1);
			uint32_t color;
			switch (color_id) {
				case 0: color = 0xFFFFFFFF; break;  // White
				case 1: color = 0xAAAAAAFF; break;  // Light Gray
				case 2: color = 0x555555FF; break;  // Dark Gray
				case 3: color = 0x000000FF; break;  // Black
				default: color = 0xFF10F0FF; break;
			}
			pixel_ptr[(screen_x + x) + (screen_y + y) * 160] = color;
		}
	}
}
void update_texture(CPU *cpu, SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture) {
	printf("!!ENTER UPDATE TEXTURE!!\n");
	exit(-3);
	void* pixels;
	int pitch;
	if (SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0) {
		printf("Failed to lock texture: %s\n", SDL_GetError());
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(SDL_GetWindowFromID(1));
		SDL_Quit();
		return;
	}

	uint32_t* pixel_ptr = (uint32_t*)pixels;

	// Get scroll values
	uint8_t scroll_top = cpu->map.memory[0xFF42];
	uint8_t scroll_left = cpu->map.memory[0xFF43];

	// Adjust LY to account for scroll position
	int ly = read_memory(&cpu->map, 0xFF44);
	int tile_map_y = (ly + scroll_top) / 8;  // Which row of tiles to render
	int tile_row_offset = (ly + scroll_top) % 8;  // Which row within the tile (0-7)

	// Tile base addresses
	uint16_t base_data_addr = 0x8000;
	uint16_t base_map_addr = 0x9800;

	// Starting address in tile map
	int start_addr = base_map_addr + ((scroll_top / 8) * 32) + (scroll_left / 8);

	// Only render the scanline corresponding to LY
	for (int x = 0; x < 20; x++) {  // 20 tiles horizontally
		uint8_t tile_index = cpu->map.memory[start_addr + tile_map_y * 32 + x];
		uint16_t tile_data_addr = base_data_addr + (tile_index * 16);

		// Update the tile for the current scanline (LY)
		update_tile_texture(cpu, pixel_ptr, tile_data_addr, 8 * x, tile_row_offset);
	}

	SDL_UnlockTexture(texture);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);}
void update_tile_data_texture(CPU *cpu, SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture) {
	void* pixels;
	int pitch;
	if (SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0) {
		printf("Failed to lock texture: %s\n", SDL_GetError());
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(SDL_GetWindowFromID(1));
		SDL_Quit();
		return;
	}
	uint32_t* pixel_ptr = (uint32_t*)pixels;

	// 0x8000 and 0x9000 otherwise
	uint16_t base_addr = 0x9000;
	for (int j = 0; j < 16; j++) {
		for (int i = 0; i < 20; i++) {
			int tile_index = j * 20 + i;
			uint16_t addr = base_addr + tile_index * 16;

			update_tile_texture(cpu, pixel_ptr, addr, 8 * i, 8 * j);
		}
	}
	// SDL_Delay(10);

	SDL_UnlockTexture(texture);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}
