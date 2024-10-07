#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "cpu.h"
#include "constants.h"
#include "memory.h"
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

	SDL_Window *window = SDL_CreateWindow("Gameboy", SDL_WINDOWPOS_UNDEFINED,
				       SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
	if (!window) {
		printf("Failed to open %d x %d window: %s\n", 640, 480, SDL_GetError());
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
					  SDL_TEXTUREACCESS_STREAMING, 640, 480);
	if (!texture) {
		printf("Failed to create texture: %s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}


	SDL_Delay(2000);

	load_rom(&cpu.map, "asm/tetris.gb");
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
	bool bit_7 = false;
	uint8_t *dest_8 = NULL;
	uint16_t *dest_16 = NULL;
	uint16_t val_16 = 0;
	uint8_t val_8 = 0;

	// TODO: should halt until interrupt
	// TODO: use switch... or no execute
	while(!cpu.halt) {
		get_input(&cpu);

		opcode = cpu.map.memory[cpu.pc++];
		printf("start pc: %d, opcode: 0x%x, a: %d, mem: %d\n", cpu.pc, opcode, cpu.reg.a, cpu.map.memory[cpu.pc]);

		uint8_t prefetch_8 = cpu.map.memory[cpu.pc + 1];
		uint16_t prefetch_16 = cpu.map.memory[cpu.pc + 1] | (cpu.map.memory[cpu.pc + 2] << 8);
		int8_t prefetch_signed_8 = prefetch_8;

		if (opcode >= 0x00 && opcode < 0x40) {
			// if (cpu.pc > 0x1000) return 0;
			switch (opcode) {
				// NOP
				case 0x00:
					printf("Executed NOP\n");
					break;
				// STOP
				case 0x10:
					cpu.halt = true;
					break;
				// JR NZ, e8
				case 0x20:
					if (!cpu.reg.f.zero) {
						cpu.pc += prefetch_signed_8 + 1;
					}
					break;
				case 0x30:
					if (!cpu.reg.f.carry) {
						cpu.pc += prefetch_signed_8 + 1;
					}
					break;
				// LD r16, nn
				case 0x01: case 0x11: case 0x21: case 0x31:
					dest_16 = get_target_reg_16(&cpu, opcode >> 4);
					*dest_16 = prefetch_16;
					cpu.pc += 2;
					break;
				// LD r16, A
				case 0x02: case 0x12:
					dest_16 = get_target_reg_16(&cpu, opcode >> 4);
					*dest_16 = cpu.reg.a;
					break;
				// LD (HL+),A
				case 0x22:
					dest_16 = get_target_reg_16(&cpu, HL);
					write_memory(&cpu.map, *dest_16, cpu.reg.a);
					(*dest_16)++;
					break;
				// LD (HL-), A
				case 0x32:
					dest_16 = get_target_reg_16(&cpu, HL);
					write_memory(&cpu.map, *dest_16, cpu.reg.a);
					(*dest_16)--;
					break;
				// INC r16
				case 0x03: case 0x13: case 0x23: case 0x33:
					dest_16 = get_target_reg_16(&cpu, opcode >> 4);
					(*dest_16)++;
					break;
				// INC B - INC D - INC H
				case 0x04: case 0x14: case 0x24:
					// TODO: Flags
					dest_8 = get_target_reg_8(
						&cpu,
						decode_register_8_pairs(opcode >> 4, true)
					);
					(*dest_8)++;
					break;
				// INC HL
				case 0x34:
					dest_16 = get_target_reg_16(&cpu, HL);
					(*dest_16)++;
					break;
				// DEC B - DEC D - DEC H
				case 0x05: case 0x15: case 0x25:
					// TODO: Flags
					dest_8 = get_target_reg_8(
						&cpu,
						decode_register_8_pairs(opcode >> 4, true)
					);
					(*dest_8)--;
					break;
				// DEC HL
				case 0x35:
					dest_16 = get_target_reg_16(&cpu, HL);
					(*dest_16)--;
					break;
				// LD B, n8 - LD D, n8 - LD H, n8
				case 0x06: case 0x16: case 0x26:
					dest_8 = get_target_reg_8(
						&cpu,
						decode_register_8_pairs(opcode >> 4, true)
					);
					*dest_8 = prefetch_8;
					cpu.pc++;
					break;
				// LD HL, n8
				case 0x36:
					dest_16 = get_target_reg_16(
						&cpu,
						HL
					);
					*dest_16 = prefetch_8;
					cpu.pc++;
					break;
				// RLCA
				case 0x07:
					cpu.reg.f.carry = (cpu.reg.a & 0xFF) != 0;

					val_8 = (cpu.reg.a << 1);
					if (cpu.reg.f.carry) val_8 |= 0x01;
					cpu.reg.a = val_8;

					cpu.reg.f.zero = val_8 == 0;
					cpu.reg.f.subtract = false;
					cpu.reg.f.half_carry = false;
					break;
				// RLA
				case 0x17:
					bit_7 = (cpu.reg.a & 0xFF) != 0;

					val_8 = (cpu.reg.a << 1);
					if (cpu.reg.f.carry) val_8 |= 0x01;
					if (bit_7) cpu.reg.f.carry = true;
					cpu.reg.a = val_8;

					cpu.reg.f.zero = val_8 == 0;
					cpu.reg.f.subtract = false;
					cpu.reg.f.half_carry = false;
					break;
				// DAA
				case 0x27:
					break;
				// SCF
				case 0x37:
					break;
				// LD a16, SP 
				case 0x08:
					break;
				// JR e8
				case 0x18:
					cpu.pc += prefetch_signed_8 + 1;
					break;
				// JR Z, e8
				case 0x28:
					if (cpu.reg.f.zero) {
						cpu.pc += prefetch_signed_8 + 1;
					}
					break;
				// JR C, e8
				case 0x38:
					if (cpu.reg.f.carry) {
						cpu.pc += prefetch_signed_8 + 1;
					}
					break;
				// ADD HL, r16
				case 0x09: case 0x19: case 0x29: case 0x39:
					dest_16 = get_target_reg_16(&cpu, opcode >> 4);
					(*dest_16) += get_reg_hl(&cpu.reg);

					cpu.reg.f.subtract = false;
					// TODO: Set carry flags
					// cpu.reg.f.carry;
					// cpu.reg.f.half_carry;
					break;
				// LD A, BC - LD A, DE
				case 0x0A: case 0x1A:
					dest_16 = get_target_reg_16(&cpu, opcode >> 4);
					cpu.reg.a = *dest_16;
					break;
				// LD A, (HL+)
				case 0x2A:
					dest_16 = get_target_reg_16(&cpu, HL);
					val_8 = read_memory(&cpu.map, *dest_16);
					cpu.reg.a = val_8;
					(*dest_16)++;
					break;
				// LD A, (HL-)
				case 0x3A:
					dest_16 = get_target_reg_16(&cpu, HL);
					val_8 = read_memory(&cpu.map, *dest_16);
					cpu.reg.a = val_8;
					(*dest_16)--;
					break;
				// DEC r16
				case 0x0B: case 0x1B: case 0x2B: case 0x3B:
					dest_16 = get_target_reg_16(&cpu, opcode >> 4);
					(*dest_16)--;
					break;
				// INC C - INC E - INC L - INC A
				case 0x0C: case 0x1C: case 0x2C: case 0x3C:
					dest_8 = get_target_reg_8(
						&cpu,
						decode_register_8_pairs(opcode >> 4, false)
					);
					(*dest_8)++;
					break;
				// DEC C - DEC E - DEC L - DEC A
				case 0x0D: case 0x1D: case 0x2D: case 0x3D:
					dest_8 = get_target_reg_8(
						&cpu,
						decode_register_8_pairs(opcode >> 4, false)
					);
					(*dest_8)--;
					break;
				// LD C, n8 - LD E, n8 - LD L, n8 - LD A, n8
				case 0x0E: case 0x1E: case 0x2E: case 0x3E:
					dest_8 = get_target_reg_8(
						&cpu,
						decode_register_8_pairs(opcode >> 4, false)
					);
					cpu.pc++;
					*dest_8 = prefetch_8;
					break;
				// RRCA
				case 0x0F:
					break;
				// RRA
				case 0x1F:
					break;
				// CPL
				case 0x2F:
					break;
				// CCF
				case 0x3F:
					break;
			}
		}
		// Shortcut for load instructions
		else if (opcode >= 0x40 && opcode < 0x80) {
			switch (opcode) {
				// HALT
				case 0x76:
					printf("Executing HALT\n");
					cpu.halt = true;
					break;
				// LD B, HL
				case 0x46:
					cpu.reg.b = read_memory(&cpu.map, get_reg_hl(&cpu.reg));
					break;
				// LD D, HL
				case 0x56:
					cpu.reg.d = read_memory(&cpu.map, get_reg_hl(&cpu.reg));
					break;
				// LD H, HL
				case 0x66:
					cpu.reg.h = read_memory(&cpu.map, get_reg_hl(&cpu.reg));
					break;
				// LD C, HL
				case 0x4E:
					cpu.reg.c = read_memory(&cpu.map, get_reg_hl(&cpu.reg));
					break;
				// LD E, HL
				case 0x5E:
					cpu.reg.e = read_memory(&cpu.map, get_reg_hl(&cpu.reg));
					break;
				// LD L, HL
				case 0x6E:
					cpu.reg.l = read_memory(&cpu.map, get_reg_hl(&cpu.reg));
					break;
				// LD A, HL
				case 0x7E:
					cpu.reg.a = read_memory(&cpu.map, get_reg_hl(&cpu.reg));
					break;
				case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75:
					{
						int src = opcode & 0xF;
						Target_8 reg_src = decode_register_8(src);
						uint8_t val = *get_target_reg_8(&cpu, reg_src);
						write_memory(&cpu.map, get_reg_hl(&cpu.reg), val);
					}
					break;
				case 0x77:
					{
						uint8_t val = *get_target_reg_8(&cpu, A);
						write_memory(&cpu.map, get_reg_hl(&cpu.reg), val);
					}
					break;
				case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // LD B, r8
				case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f: // LD C, r8
				case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // LD D, r8
				case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f: // LD E, r8
				case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // LD H, r8
				case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f: // LD L, r8
				case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f: // LD A, r8
					{
						int src = opcode & 0x7;
						int dest = (opcode >> 3) & 0x7;
						// TODO: case for reg HL, HALT
						Target_8 reg_src = decode_register_8(src);
						Target_8 reg_dest = decode_register_8(dest);

						*get_target_reg_8(&cpu, reg_dest) = *get_target_reg_8(&cpu, reg_src);
						printf("Executed LD r8, r8: opcode: 0x%x, acc: %d, src: %d - %d, dest: %d - %d\n\n", cpu.reg.a, opcode, src, *get_target_reg_8(&cpu, reg_src), dest, *get_target_reg_8(&cpu, reg_dest));
					}
					break;
			}
		}
		// Math instructions
		// Add, sub, and, or
		else if (opcode >= 0x80 && opcode < 0xC0) {
			if ((opcode & 0xF) == 0x6 || (opcode & 0xF) == 0xE) {
				switch (opcode) {
					// ADD A, HL
					case 0x86:
						add_n8(
							&cpu,
							read_memory(&cpu.map, get_reg_hl(&cpu.reg))
						);
						break;
					// SUB A, HL
					case 0x96:
						sub(
							&cpu,
							read_memory(&cpu.map, get_reg_hl(&cpu.reg))
						);
						break;
					// AND A, HL
					case 0xA6:
						and(
							&cpu,
							read_memory(&cpu.map, get_reg_hl(&cpu.reg))
						);
						break;
					// OR A, HL
					case 0xB6:
						or(
							&cpu,
							read_memory(&cpu.map, get_reg_hl(&cpu.reg))
						);
						break;
					// ADC A, HL
					case 0x8E:
						adc(
							&cpu,
							read_memory(&cpu.map, get_reg_hl(&cpu.reg))
						);
						break;
					// SBC A, HL
					case 0x9E:
						sbc(
							&cpu,
							read_memory(&cpu.map, get_reg_hl(&cpu.reg))
						);
						break;
					// XOR A, HL
					case 0xAE:
						xor(
							&cpu,
							read_memory(&cpu.map, get_reg_hl(&cpu.reg))
						);
						break;
					// CP A, HL
					case 0xBE:
						cp(
							&cpu,
							read_memory(&cpu.map, get_reg_hl(&cpu.reg))
						);
						break;
				}
			}
			else {
				int src = opcode & 0x7;
				int operation = (opcode >> 3) & 0x7;
				Target_8 reg_src = decode_register_8(src);

				execute_instruction(&cpu, operation, reg_src, BC, -1, -1);
				printf("Executed MATH: opcode: 0x%x, operation: %d, acc: %d, src: %d - %d\n", opcode, operation, cpu.reg.a, src, *get_target_reg_8(&cpu, reg_src));
			}
		}
		else if (opcode >= 0xC0 && opcode <= 0xFF) {
			switch (opcode) {
				// RET NZ
				case 0xC0:
					if (!cpu.reg.f.zero) {
						val_16 = pop_n16(&cpu);
						jp(&cpu, val_16);
					}
					break;
				// RET NC
				case 0xD0:
					if (!cpu.reg.f.carry) {
						val_16 = pop_n16(&cpu);
						jp(&cpu, val_16);
					}
					break;
				// LDH a8, A
				case 0xE0:
					break;
				// LDH A, a8
				case 0xF0:
					break;
				// POP r16
				case 0xC1: case 0xD1: case 0xE1: case 0xF1:
					// TODO: Fix case F1, AF not implemented
					pop_r16(&cpu, decode_register_16((opcode - 0xC0) >> 4));
					break;
				// JP NZ, a16
				case 0xC2:
					if (!cpu.reg.f.zero) {
						cpu.pc = prefetch_16;
					}
					break;
				// JP NC, a16
				case 0xD2:
					if (!cpu.reg.f.carry) {
						cpu.pc = prefetch_16;
					}
					break;
				// LD (C), A
				case 0xE2:
					write_memory(&cpu.map, 0xFF00 + cpu.reg.c, cpu.reg.a);
					break;
				// LD A, (C)
				case 0xF2:
					cpu.reg.a = read_memory(&cpu.map, 0xFF00 + cpu.reg.c);
					break;
				// JP a16
				case 0xC3:
					cpu.pc = prefetch_16;
					break;
				// DI
				case 0xF3:
					cpu.ime = false;
					break;
				// CALL NZ, a16
				case 0xC4:
					if (!cpu.reg.f.zero) {
						push_n16(
							&cpu,
							read_memory(&cpu.map, cpu.pc + 1)
						);
						cpu.pc = prefetch_16;
					}
					break;
				// CALL NC, a16
				case 0xD4:
					if (!cpu.reg.f.carry) {
						push_n16(
							&cpu,
							read_memory(&cpu.map, cpu.pc + 1)
						);
						cpu.pc = prefetch_16;
					}
					break;
				// PUSH r16
				case 0xC5: case 0xD5: case 0xE5: case 0xF5:
					// TODO: Fix case F5, AF not implemented
					push_r16(&cpu, decode_register_16((opcode - 0xC0) >> 4));
					break;
				// ADD A, n8
				case 0xC6: 
					add_n8(&cpu, prefetch_8);
					cpu.pc++;
					break;
				// SUB A, n8
				case 0xD6:
					sub(&cpu, prefetch_8);
					cpu.pc++;
					break;
				// AND A, n8
				case 0xE6:
					and(&cpu, prefetch_8);
					cpu.pc++;
					break;
				// OR A, n8
				case 0xF6:
					or(&cpu, prefetch_8);
					cpu.pc++;
					break;
				// RST $00
				case 0xC7:
					push_n16(&cpu, cpu.pc);
					jp(&cpu, 0x00);
					break;
				// RST $10
				case 0xD7:
					push_n16(&cpu, cpu.pc);
					jp(&cpu, 0x10);
					break;
				// RST $20
				case 0xE7:
					push_n16(&cpu, cpu.pc);
					jp(&cpu, 0x20);
					break;
				// RST $30
				case 0xF7:
					push_n16(&cpu, cpu.pc);
					jp(&cpu, 0x30);
					break;
				// RET Z
				case 0xC8:
					if (cpu.reg.f.zero) {
						val_16 = pop_n16(&cpu);
						jp(&cpu, val_16);
					}
					break;
				// RET C
				case 0xD8:
					if (cpu.reg.f.carry) {
						val_16 = pop_n16(&cpu);
						jp(&cpu, val_16);
					}
					break;
				// ADD SP, e8
				case 0xE8:
					cpu.sp += prefetch_signed_8;
					cpu.pc++;
					break;
				// LD HL, SP + e8
				case 0xF8:
					set_reg_hl(&cpu.reg, cpu.sp + prefetch_signed_8);
					cpu.pc++;
					break;
				// RET
				case 0xC9:
					val_16 = pop_n16(&cpu);
					jp(&cpu, val_16);
					break;
				// RETI
				case 0xD9:
					val_16 = pop_n16(&cpu);
					jp(&cpu, val_16);
					cpu.ime = true;
					break;
				// JP HL
				case 0xE9:
					jp(&cpu, get_reg_hl(&cpu.reg));
					break;
				// LD SP, HL
				case 0xF9:
					cpu.sp = get_reg_hl(&cpu.reg);
					break;
				// JP Z, a16
				case 0xCA:
					if (cpu.reg.f.zero) {
						cpu.pc = prefetch_16;
					}
					break;
				// JP C, a16
				case 0xDA:
					if (cpu.reg.f.carry) {
						cpu.pc = prefetch_16;
					}
					break;
				// LD a16, A
				case 0xEA:
					write_memory(&cpu.map, prefetch_16, cpu.reg.a);
					cpu.pc += 2;
					break;
				// LD A, a16
				case 0xFA:
					cpu.reg.a = read_memory(&cpu.map, prefetch_16);
					cpu.pc += 2;
					break;
				// PREFIX for color gameboy opcodes
				case 0xCB:
					break;
				// EI
				case 0xFB:
					cpu.ime = true;
					break;
				// CALL Z, a16
				case 0xCC:
					if (cpu.reg.f.zero) {
						push_n16(
							&cpu,
							read_memory(&cpu.map, cpu.pc + 1)
						);
						cpu.pc = prefetch_16;
					}
					break;
				// CALL C, a16
				case 0xDC:
					if (cpu.reg.f.carry) {
						push_n16(
							&cpu,
							read_memory(&cpu.map, cpu.pc + 1)
						);
						cpu.pc = prefetch_16;
					}
					break;
				// CALL a16
				case 0xCD:
					push_n16(
						&cpu,
						read_memory(&cpu.map, cpu.pc + 1)
					);
					cpu.pc = prefetch_16;
					break;
				// ADC A, n8
				case 0xCE:
					adc(&cpu, prefetch_8);
					cpu.pc++;
					break;
				// SBC A, n8
				case 0xDE:
					sbc(&cpu, prefetch_8);
					cpu.pc++;
					break;
				// XOR A, n8
				case 0xEE:
					xor(&cpu, prefetch_8);
					cpu.pc++;
					break;
				// CP A, n8
				case 0xFE:
					cp(&cpu, prefetch_8);
					cpu.pc++;
					break;
				// RST $08
				case 0xCF:
					push_n16(&cpu, cpu.pc);
					jp(&cpu, 0x08);
					break;
				// RST $18
				case 0xDF:
					push_n16(&cpu, cpu.pc);
					jp(&cpu, 0x18);
					break;
				// RST $28
				case 0xEF:
					push_n16(&cpu, cpu.pc);
					jp(&cpu, 0x28);
					break;
				// RST $38
				case 0xFF:
					push_n16(&cpu, cpu.pc);
					jp(&cpu, 0x38);
					break;
				// Unused instructions
				case 0xD3: case 0xE3: case 0xE4: case 0xF4: case 0xDB: case 0xEB: case 0xEC:
				case 0xFC: case 0xDD: case 0xED: case 0xFD:
					printf("[ERROR] - Unused instruction: 0x%X", opcode);
					break;
			}

			printf("Executed STACK: opcode: 0x%x\n", opcode);
		}
		else {
			printf("[ERROR] - Unknown instruction 0x%x\n", opcode);
		}
		printf("end pc: %d, opcode: 0x%x, a: %d, mem: %d\n", cpu.pc, opcode, cpu.reg.a, cpu.map.memory[cpu.pc]);

		update_texture(&cpu, window, renderer, texture);
		render(renderer, texture);
	} 

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
