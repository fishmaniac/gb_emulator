#include "memory.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

void write_memory(MemoryMap *map, uint16_t addr, uint8_t val) {
	switch (addr) {
		case 0xFF04:
			map->memory[DIV_ADDR] = 0;
			break;
		default:
			map->memory[addr] = val;
			break;
	}
	printf("WRITE MEM[0x%x] = 0x%x\n", addr, map->memory[addr]);
}

uint8_t read_memory(MemoryMap *map, uint16_t addr) {
	// Bus bus = get_bus(address);
	printf("READ MEM[0x%x] = 0x%x\n", addr, map->memory[addr]);
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

	FILE *rom = fopen(path, "rb");
	if (!rom) {
		printf("[ERROR] File %s cannot be opened\n", path);
		exit(-2);
	}

	int bytes = fread(map->memory, sizeof(uint8_t), 0x8000, rom);
	printf("Read %d bytes from %s\n", bytes, path);
	printf("ROM size = %d KiB\n", 32 * (1 << map->memory[0x148]));
	printf("RAM size = %d\n", map->memory[0x149]);
	printf("Country = %s\n", map->memory[0x14A] == 0 ? "Japan" : "Overseas only");
	printf("License Code = %d\n", map->memory[0x14B]);
	printf("Version = %d\n", map->memory[0x14C]);
	// int i = 0;
	// uint8_t byte;
	// while (fread(&byte, sizeof(uint8_t), 1, rom) && i < 131072) {
	// 	printf("Byte: 0x%X\n", byte);
	// 	map->rom[i++] = byte;
	// }

	fclose(rom);
}
