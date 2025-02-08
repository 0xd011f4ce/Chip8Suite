#ifndef __REGISTERS_H
#define __REGISTERS_H

#include "config.h"

struct chip8_registers
{
	unsigned char V[CHIP8_TOTAL_DATA_REGISTERS];
	unsigned short I;
	unsigned char DT;	 // Delay timer
	unsigned char ST;	 // Delay timer
	unsigned short PC; // Program counter
	unsigned char SP;	 // Stack pointer
};

#endif
