#ifndef TIMER_H
#define TIMER_H

#include "cpu.h"
#include "memory.h"
#include <stdbool.h>

static const int CPU_FREQ = 4194304;			// 4.19 MHz
static const int DIV_FREQ = 16384;			// 16384 Hz
static const uint16_t DIV_ADDR = 0xFF04;

static const int DIV_CYCLE_INTERVAL = CPU_FREQ / DIV_FREQ;
// Not sure about this
static const int CYCLES_PER_SCANLINE = 456;

static const int TIMA_00_FREQ = 4096;
static const int TIMA_01_FREQ = 262144;
static const int TIMA_10_FREQ = 65536;
static const int TIMA_11_FREQ = 16384;
static const uint16_t TAC_ADDR = 0xFF07;
static const uint8_t TAC_CLOCK_BIT = 0x3;
static const uint16_t TMA_ADDR = 0xFF06;
static const uint16_t TIMA_ADDR = 0xFF05;

// Interrupt flag
static const uint16_t IF_ADDR = 0xFF0F;
// Interrupt Enable
static const uint16_t IE_ADDR = 0xFFFF;

static const uint8_t VBLANK_BIT = 0x1;
static const uint8_t VBLANK_BIT_POS = 0;
static const uint8_t LCD_BIT = 0x2;
static const uint8_t LCD_BIT_POS = 1;
static const uint8_t TIMER_BIT = 0x4;
static const uint8_t TIMER_BIT_POS = 2;
static const uint8_t SERIAL_BIT = 0x8;
static const uint8_t SERIAL_BIT_POS = 3;
static const uint8_t JOYPAD_BIT = 0x10;
static const uint8_t JOYPAD_BIT_POS = 4;

static const uint16_t VBLANK_HANDLER = 0x40;
static const uint16_t STAT_HANDLER = 0x48;
static const uint16_t TIMER_HANDLER = 0x50;
static const uint16_t SERIAL_HANDLER = 0x58;
static const uint16_t JOYPAD_HANDLER = 0x60;

typedef enum {
	VBlank,
	STAT,	// LCD
	Timer,
	Serial,
	Joypad,
} Interrupt;

Interrupt get_requested_interrupt(MemoryMap *map);
Interrupt handle_interrupt(CPU *cpu);
void request_interrupt(MemoryMap *map, Interrupt interrupt);
uint8_t get_interrupt_flag(MemoryMap *map);
uint8_t get_tima_modulo(MemoryMap *map);
void increment_tima(MemoryMap *map);
bool get_tac_enable(MemoryMap *map);
int get_tac_clock(MemoryMap *map);

#endif	// TIMER_H
