#include "memory.h"
#include "timer.h"
#include <stdio.h>

void write_memory(MemoryMap *map, uint16_t addr, uint8_t val) {
	switch (addr) {
		case 0xFF04:
			map->memory[DIV_ADDR] = 0;
			break;
		default:
			map->memory[addr] = val;
			break;
	}
}

uint8_t read_memory(MemoryMap *map, uint16_t addr) {
	// Bus bus = get_bus(address);

	return map->memory[addr];
}

void load_rom(MemoryMap *map, int bank) {
	for (int i = 0; i < 0x4000; i++) {
		map->memory[i] = map->rom[i];
	}
	set_bank(map, bank);
}


void set_bank(MemoryMap *map, int bank) {
	int addr = bank * 0x4000;

	for (int rom_addr = 0x4000 + addr, mem_addr = 0x4000; rom_addr < 0x8000 + addr; rom_addr++, mem_addr++) {
		map->memory[mem_addr] = map->rom[rom_addr];
	}
}

void read_simple_rom(MemoryMap *map) {
	char *file = "asm/simple.bin";
	int i = 0x100;

	FILE *rom = fopen(file, "rb");
	if (!rom) {
		printf("[ERROR] File %s cannot be opened", file);
	}

	uint8_t byte;
	while (fread(&byte, sizeof(uint8_t), 1, rom)) {
		// printf("Byte: 0x%X\n", byte);
		map->rom[i++] = byte;
	}

	fclose(rom);
}

void read_rom(MemoryMap *map, char* path) {
	int i = 0;

	FILE *rom = fopen(path, "rb");
	if (!rom) {
		printf("[ERROR] File %s cannot be opened", path);
	}

	uint8_t byte;
	while (fread(&byte, sizeof(uint8_t), 1, rom) && i < 131072) {
		if (i < 524) printf("Byte: 0x%X\n", byte);
		map->rom[i++] = byte;
	}

	fclose(rom);
}
