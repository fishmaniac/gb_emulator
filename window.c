#include "window.h"
#include "cpu.h"
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

void update_texture(CPU *cpu, SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture) {
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

	Uint32* pixel_ptr = (Uint32*)pixels;
	int screen_width = 160;
	int screen_height = 144;
	int TILE_HEIGHT = 8;
	int TILE_WIDTH = 8;

	uint16_t tile_data_addr = 0x8000;
	uint16_t bg_map_addr = 0x9800;


	// Iterate over the visible screen (20x18 tiles, 160x144 pixels)
	for (int tile_y = 0; tile_y < screen_height / TILE_HEIGHT; ++tile_y) {
		for (int tile_x = 0; tile_x < screen_width / TILE_WIDTH; ++tile_x) {
			// Get the tile index from the background map
			uint8_t tile_index = cpu->map.memory[bg_map_addr + tile_y * 32 + tile_x];

			// Get the address of the tile data
			uint16_t tile_addr = tile_data_addr + tile_index * 16;

			// Render each tile (8x8 pixels)
			for (int y = 0; y < TILE_HEIGHT; ++y) {
				// Each row of a tile consists of two bytes (low and high)
				uint8_t byte1 = cpu->map.memory[tile_addr + y * 2];
				uint8_t byte2 = cpu->map.memory[tile_addr + y * 2 + 1];

				for (int x = 0; x < TILE_WIDTH; ++x) {
					// Extract the color bits (2 bits per pixel)
					int bit = 7 - x;  // Work through each bit from left to right
					uint8_t color_id = ((byte2 >> bit) & 1) << 1 | ((byte1 >> bit) & 1);

					// Map the 2-bit color ID to actual colors (you can adjust these values)
					Uint32 color;
					switch (color_id) {
						case 0: color = 0xFFFFFFFF; break;  // White
						case 1: color = 0xAAAAAAFF; break;  // Light Gray
						case 2: color = 0x555555FF; break;  // Dark Gray
						case 3: color = 0x000000FF; break;  // Black
					}

					// Set the pixel in the SDL texture (texture is 640x480, Game Boy screen is 160x144)
					int screen_pixel_x = tile_x * TILE_WIDTH + x;
					int screen_pixel_y = tile_y * TILE_HEIGHT + y;

					// Scale up the pixel to fit the 640x480 SDL window (if you're scaling up)
					int scaled_x = screen_pixel_x * (640 / screen_width);
					int scaled_y = screen_pixel_y * (480 / screen_height);

					// Plot scaled pixel
					for (int sy = 0; sy < (480 / screen_height); ++sy) {
						for (int sx = 0; sx < (640 / screen_width); ++sx) {
							pixel_ptr[(scaled_y + sy) * (pitch / 4) + (scaled_x + sx)] = color;
						}
					}
				}
			}
		}
	}

	SDL_UnlockTexture(texture);

	// Render the texture to the screen
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);}
