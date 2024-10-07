#include "memory.h"
#include <stdio.h>

// TODO: Use this to switch between different memory locations
static Bus get_bus(uint16_t address) {
	if (address < VRAM_START) {
		return BUS_RAM;
	}
	else if (address < EXTERNAL_RAM_START) {
		return BUS_VRAM;
	}
	else {
		return BUS_RAM;
	}
}

void write_memory(MemoryMap *map, uint16_t address, uint8_t val) {
	map->memory[address] = val;
}

uint8_t read_memory(MemoryMap *map, uint16_t address) {
	// Bus bus = get_bus(address);

	return map->memory[address];
}

void load_simple_rom(MemoryMap *map) {
	char *file = "asm/simple.bin";
	int i = 0x100;

	FILE *rom = fopen(file, "rb");
	if (!rom) {
		printf("[ERROR] File %s cannot be opened", file);
	}

	uint8_t byte;
	while (fread(&byte, sizeof(uint8_t), 1, rom)) {
		// printf("Byte: 0x%X\n", byte);
		map->memory[i++] = byte;
	}

	fclose(rom);
}

void load_rom(MemoryMap *map, char* path) {
	int i = 0;

	FILE *rom = fopen(path, "rb");
	if (!rom) {
		printf("[ERROR] File %s cannot be opened", path);
	}

	uint8_t byte;
	while (fread(&byte, sizeof(uint8_t), 1, rom)) {
		printf("Byte: 0x%X\n", byte);
		map->memory[i++] = byte;
	}

	fclose(rom);
}
void read_rom(MemoryMap *map) {
	uint16_t opcode = 0xFFFF;

}

