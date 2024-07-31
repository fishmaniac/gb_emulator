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
	Bus bus = get_bus(address);

	return map->memory[address];
}
