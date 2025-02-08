#include "mem.h"

#include <assert.h>

static void
chip8_is_mem_in_bounds (int index)
{
	assert (index >= 0 && index < CHIP8_MEMORY_SIZE);
}

void
chip8_memory_set (struct chip8_memory *mem, int index, unsigned char val)
{
	chip8_is_mem_in_bounds (index);
	mem->memory[index] = val;
}

unsigned char
chip8_memory_get (struct chip8_memory *mem, int index)
{
	chip8_is_mem_in_bounds (index);
	return mem->memory[index];
}
