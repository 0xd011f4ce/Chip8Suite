#ifndef __CHIP8_H
#define __CHIP8_H

#include "config.h"
#include "keyboard.h"
#include "mem.h"
#include "registers.h"
#include "stack.h"

struct chip8
{
	struct chip8_memory memory;
	struct chip8_registers registers;
	struct chip8_stack stack;
	struct chip8_keyboard keyboard;
};

#endif
