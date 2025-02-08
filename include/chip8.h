#ifndef __CHIP8_H
#define __CHIP8_H

#include "config.h"
#include "mem.h"
#include "registers.h"

struct chip8
{
	struct chip8_memory memory;
	struct chip8_registers registers;
};

#endif
