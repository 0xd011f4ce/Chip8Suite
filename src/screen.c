#include "screen.h"

#include <assert.h>
#include <memory.h>

static void
chip8_screen_ensure_in_bounds (int x, int y)
{
	assert (x >= 0 && x <= CHIP8_DISPLAY_WIDTH);
	assert (y >= 0 && y <= CHIP8_DISPLAY_HEIGHT);
}

void
chip8_screen_clear (struct chip8_screen *screen)
{
	memset (screen->pixels, 0, sizeof (screen->pixels));
}

void
chip8_screen_set (struct chip8_screen *screen, int x, int y)
{
	chip8_screen_ensure_in_bounds (x, y);
	screen->pixels[y][x] = 1;
}

_Bool
chip8_screen_is_set (struct chip8_screen *screen, int x, int y)
{
	chip8_screen_ensure_in_bounds (x, y);
	return screen->pixels[y][x];
}

_Bool
chip8_screen_draw_sprite (struct chip8_screen *screen, int x, int y,
													const char *sptr, int num)
{
	_Bool sprite_hit = 0; // true if sptr overrides a set bit

	for (int ly = 0; ly < num; ly++)
		{
			char c = sptr[ly];
			for (int lx = 0; lx < 8; lx++)
				{
					if ((c & (0b10000000 >> lx)) == 0)
						continue;

					if (screen->pixels[(ly + y) % CHIP8_DISPLAY_HEIGHT]
														[(lx + x) % CHIP8_DISPLAY_WIDTH])
						{
							sprite_hit = 1;
						}

					// the % wraps it around the screen
					screen->pixels[(ly + y) % CHIP8_DISPLAY_HEIGHT]
												[(lx + x) % CHIP8_DISPLAY_WIDTH]
							^= 1;
				}
		}

	return sprite_hit;
}
