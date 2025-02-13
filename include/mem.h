#ifndef __MEM_H
#define __MEM_H

#include "config.h"

struct chip8_memory
{
	unsigned char memory[CHIP8_MEMORY_SIZE];
};

void chip8_memory_set (struct chip8_memory *mem, int index, unsigned char val);
unsigned char chip8_memory_get (struct chip8_memory *mem, int index);
unsigned short chip8_memory_get_short (struct chip8_memory *mem, int index);

#endif
