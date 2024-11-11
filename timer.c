#include "timer.h"
#include "cpu.h"
#include "memory.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

Interrupt get_requested_interrupt(MemoryMap *map) {
	uint8_t flag = get_interrupt_flag(map);

	if (flag & VBLANK_BIT) {
		printf("!!VBLANK INTERRUPT!!");
		exit(-3);
		return VBlank;
	}
	else if ((flag & LCD_BIT) >> LCD_BIT_POS) {
		return STAT;
	}
	else if ((flag & TIMER_BIT) >> TIMER_BIT_POS) {
		return Timer;
	}
	else if ((flag & SERIAL_BIT) >> SERIAL_BIT_POS) {
		return Serial;
	}
	else if ((flag & JOYPAD_BIT) >> JOYPAD_BIT_POS) {
		return Joypad;
	}
	else {
		return -1;
	}
}

Interrupt handle_interrupt(CPU *cpu) {
	if (!cpu->ime) return -1;

	Interrupt interrupt = get_requested_interrupt(&cpu->map);
	uint8_t flag = get_interrupt_flag(&cpu->map);

	switch (interrupt) {
		case VBlank:
			flag &= ~(1 << VBLANK_BIT_POS);
			call_n16(cpu, VBLANK_HANDLER);
		case STAT:
			flag &= ~(1 << LCD_BIT_POS);
			call_n16(cpu, STAT_HANDLER);
		case Timer:
			flag &= ~(1 << TIMER_BIT_POS);
			call_n16(cpu, TIMER_HANDLER);
		case Serial:
			flag &= ~(1 << SERIAL_BIT_POS);
			call_n16(cpu, SERIAL_HANDLER);
		case Joypad:
			flag &= ~(1 << JOYPAD_BIT_POS);
			call_n16(cpu, JOYPAD_HANDLER);
	}

	write_memory(&cpu->map, IF_ADDR, flag);

	cpu->ime = false;

	return interrupt;
}
void request_interrupt(MemoryMap *map, Interrupt interrupt) {
	uint8_t flag = get_interrupt_flag(map);

	switch (interrupt) {
		case VBlank:
			flag |= VBLANK_BIT;
			break;
		case STAT:
			flag |= LCD_BIT;
			break;
		case Timer:
			flag |= TIMER_BIT;
			break;
		case Serial:
			flag |= SERIAL_BIT;
			break;
		case Joypad:
			flag |= JOYPAD_BIT;
			break;
	}
	write_memory(map, IF_ADDR, flag);
}
uint8_t get_interrupt_flag(MemoryMap *map) {
	return read_memory(map, IF_ADDR);
}

uint8_t get_tima_modulo(MemoryMap *map) {
	return read_memory(map, TMA_ADDR);
}
void increment_tima(MemoryMap *map) {
	uint8_t tima = read_memory(map, TIMA_ADDR);
	bool tac = get_tac_enable(map);

	if (tac && tima > 0xFF) {
		write_memory(map, TIMA_ADDR, get_tima_modulo(map));
		request_interrupt(map, Timer);
		// if ((read_memory(map, IE_ADDR) & TIMER_BIT) >> TIMER_BIT_POS) {
		// 	uint8_t interrupt = TIMER_BIT | get_interrupt_flag(map);
		// 	write_memory(map, IF_ADDR, interrupt);
		// }
	}
	else if (tac) {
		write_memory(map, TIMA_ADDR, tima + 1);
	}
}
bool get_tac_enable(MemoryMap *map) {
	return (read_memory(map, TAC_ADDR) & TIMER_BIT) >> TIMER_BIT_POS;
}
// TODO: writing to TAC_ADDR may increment TIMA_ADDR once
int get_tac_clock(MemoryMap *map) {
	uint8_t clock = read_memory(map, TAC_ADDR) & TAC_CLOCK_BIT;
	switch (clock) {
		case 0:
			return TIMA_00_FREQ;
		case 0x1:
			return TIMA_01_FREQ;
		case 0x2:
			return TIMA_10_FREQ;
		case 0x3:
			return TIMA_11_FREQ;
		default:
			printf("[ERROR] - INVALID TIMA CLOCK SELECT\n");
			return -1;
	}
}


