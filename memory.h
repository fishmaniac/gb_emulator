#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define MEMORY_SIZE 65535	// 0xFFFF

static const uint16_t ROM_00_START = 0x0000;		// 0
static const uint16_t ROM_00_END = 0x3FFF;		// 16383
static const uint16_t ROM_NN_START = 0x4000;		// 16384
static const uint16_t ROM_NN_END = 0x7FFF;		// 32767
static const uint16_t VRAM_START = 0x8000;		// 32768
static const uint16_t VRAM_END = 0x9FFF;		// 40959
static const uint16_t EXTERNAL_RAM_START = 0xA000;	// 40960
static const uint16_t EXTERNAL_RAM_END = 0xBFFF;	// 49151
static const uint16_t SWITCHABLE_RAM_START = 0xC000;	// 49152

// Port/Mode Registers
static const uint16_t P1 = 0xFF00;
static const uint16_t SB = 0xFF01;
static const uint16_t SC = 0xFF02;
static const uint16_t DIV = 0xFF04;
static const uint16_t TIMA = 0xFF05;
static const uint16_t TMA = 0xFF06;
static const uint16_t TAC = 0xFF07;

// Interrupt Flags
static const uint16_t IF = 0xFF0F;
static const uint16_t IE = 0xFFFF;
// static const uint16_t IME = ?

// LCD Display Registers
static const uint16_t LCDC = 0xFF40;
static const uint16_t STAT = 0xFF41;
static const uint16_t SCY = 0xFF42;
static const uint16_t SCX = 0xFF43;
static const uint16_t LY = 0xFF44;
static const uint16_t LYC = 0xFF45;
static const uint16_t DMA = 0xFF46;
static const uint16_t BGP = 0xFF47;
static const uint16_t OBP0 = 0xFF48;
static const uint16_t OBP1 = 0xFF49;
static const uint16_t WY = 0xFF4A;
static const uint16_t WX = 0xFF4B;
static const uint16_t OAM_START = 0xFE00;
static const uint16_t OAM_END = 0xFE9F;


// TODO: Map E000-FE00 to C000-DE00 (these addresses share memory)

typedef enum Bus_t {
	BUS_RAM, // Bus for RAM and Cartridge
	BUS_VRAM,
} Bus;

static Bus get_bus(uint16_t address);

typedef struct Memory_t {
	uint8_t memory[MEMORY_SIZE];
} MemoryMap;

void write_memory(MemoryMap *map, uint16_t address, uint8_t val);
uint8_t read_memory(MemoryMap *map, uint16_t address);

void load_simple_rom(MemoryMap *map);
void load_rom(MemoryMap *map);

void read_rom(MemoryMap *map);

#endif	// MEMORY_H
