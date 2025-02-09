#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "config.h"

struct chip8_keyboard
{
	_Bool keys[CHIP8_TOTAL_KEYS];
};

int chip8_keyboard_map (const char *map, char key);
void chip8_keyboard_down (struct chip8_keyboard *kbd, int key);
void chip8_keyboard_up (struct chip8_keyboard *kbd, int key);
_Bool chip8_keyboard_is_down (struct chip8_keyboard *kbd, int key);

#endif
