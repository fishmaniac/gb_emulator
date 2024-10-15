#ifndef WINDOW_H
#define WINDOW_H

#include "cpu.h"
#include <SDL2/SDL.h>

void get_input(CPU *cpu);
void render(SDL_Renderer *renderer, SDL_Texture *texture);
void update_texture(CPU *cpu, SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture);
void update_tile_data_texture(CPU *cpu, SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture);

#endif	// WINDOW_H
