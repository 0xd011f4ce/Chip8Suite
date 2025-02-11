#ifndef __SCREEN_H
#define __SCREEN_H

#include "config.h"

struct chip8_screen
{
	_Bool pixels[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
};

void chip8_screen_set (struct chip8_screen *screen, int x, int y);
_Bool chip8_screen_is_set (struct chip8_screen *screen, int x, int y);
_Bool chip8_screen_draw_sprite (struct chip8_screen *screen, int x, int y,
																const char *sptr, int num);

#endif
