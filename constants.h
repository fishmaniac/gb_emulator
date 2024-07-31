#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

#undef UINT8_MAX
#undef UINT16_MAX

static const int UINT4_MAX = 15;	// 0xF
static const int UINT8_MAX = 255;	// 0xFF
static const int UINT12_MAX = 4095;	// 0xFFF
static const int UINT16_MAX = 65535;	// 0xFFFF

static const int RAM_SIZE = 8192; // 8KB
static const int VRAM_SIZE = 8192; // 8KB
static const int RESOLUTION_X = 160; // 160 px / 20 tiles
static const int RESOLUTION_Y = 144; // 144 px / 18 tiles

/* 
 * Max # of sprites: 40
 * ï Max # sprites/line: 10
 * ï Max sprite size: 8x16
 * ï Min sprite size: 8x8
 * ï Clock Speed: 4.194304 MHz
 * (4.295454 SGB, 4.194/8.388MHz GBC)
 * ï Horiz Sync: 9198 KHz (9420 KHz for SGB)
 * ï Vert Sync: 59.73 Hz (61.17 Hz for SGB)
 * ï Sound: 4 channels with stereo sound
 * ï Power: DC6V 0.7W (DC3V 0.7W for GB Pocket)
 * 1 machine cycle = 4 clock cycles
 * GB CPU Speed NOP Instruction
 * Machine Cycles 1.05MHz 1 cycle
 * Clock Cycles 4.19MHz 4 cycles
 */

#endif	// CONSTANTS_H
