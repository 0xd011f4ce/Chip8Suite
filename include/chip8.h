#ifndef __CHIP8_H
#define __CHIP8_H

#include <stddef.h>

#include "config.h"
#include "keyboard.h"
#include "mem.h"
#include "registers.h"
#include "screen.h"
#include "stack.h"

struct chip8
{
	struct chip8_memory memory;
	struct chip8_registers registers;
	struct chip8_stack stack;
	struct chip8_keyboard keyboard;
	struct chip8_screen screen;
};

void chip8_init (struct chip8 *chip8);
void chip8_load (struct chip8 *chip8, const char *buf, size_t size);
void chip8_exec (struct chip8 *chip8, unsigned short opcode);

#endif
