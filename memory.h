#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define MEMORY_SIZE 65536	// 0xFFFF

static const uint16_t ROM_00_START = 0x0000;		// 0
static const uint16_t ROM_00_END = 0x3FFF;		// 16383
static const uint16_t ROM_NN_START = 0x4000;		// 16384
static const uint16_t ROM_NN_END = 0x7FFF;		// 32767
static const uint16_t VRAM_START = 0x8000;		// 32768
static const uint16_t VRAM_END = 0x9FFF;		// 40959
static const uint16_t EXTERNAL_RAM_START = 0xA000;	// 40960
static const uint16_t EXTERNAL_RAM_END = 0xBFFF;	// 49151
static const uint16_t SWITCHABLE_RAM_START = 0xC000;	// 49152


// TODO: Map E000-FE00 to C000-DE00 (these addresses share memory)

typedef enum Bus_t {
	BUS_RAM, // Bus for RAM and Cartridge
	BUS_VRAM,
} Bus;

static Bus get_bus(uint16_t address);

typedef struct Memory_t {
	uint8_t memory[MEMORY_SIZE];
	uint8_t rom[131072];	// 128 KiB (8 banks)
} MemoryMap;

void write_memory(MemoryMap *map, uint16_t address, uint8_t val);
uint8_t read_memory(MemoryMap *map, uint16_t address);

void load_rom(MemoryMap *map, int bank);

void set_bank(MemoryMap *map, int bank);

void read_simple_rom(MemoryMap *map);
void read_rom(MemoryMap *map, char* path);

#endif	// MEMORY_H
