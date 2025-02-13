#include <SDL2/SDL.h>
#include <unistd.h>

#include "chip8.h"

const char keyboard_map[CHIP8_TOTAL_KEYS]
		= { SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7,
				SDLK_8, SDLK_9, SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f };

int
main (int argc, const char **argv)
{
	if (argc < 2)
		{
			fprintf (stderr, "you must provide a file to load.\n");
			return EXIT_FAILURE;
		}

	const char *fname = argv[1];
	printf ("the file to load is: %s\n", fname);

	FILE *f = fopen (fname, "rb");
	if (!f)
		{
			fprintf (stderr, "failed to open the file.\n");
			return EXIT_FAILURE;
		}

	fseek (f, 0, SEEK_END);
	long size = ftell (f);
	fseek (f, 0, SEEK_SET);

	char buf[size];
	int res = fread (buf, size, 1, f);
	if (res != 1)
		{
			fprintf (stderr, "failed to read from file.\n");
			return EXIT_FAILURE;
		}

	struct chip8 chip8;
	chip8_init (&chip8);
	chip8_load (&chip8, buf, size);
	chip8_keyboard_set_map (&chip8.keyboard, keyboard_map);

	SDL_Init (SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow (
			EMULATOR_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			CHIP8_DISPLAY_WIDTH * EMULATOR_WINDOW_MULTIPLIER,
			CHIP8_DISPLAY_HEIGHT * EMULATOR_WINDOW_MULTIPLIER, SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer
			= SDL_CreateRenderer (window, -1, SDL_TEXTUREACCESS_TARGET);

	chip8.registers.ST = 255;
	while (1)
		{
			SDL_Event event;
			while (SDL_PollEvent (&event))
				{
					switch (event.type)
						{
						case SDL_QUIT:
							goto out;
							break;

						case SDL_KEYDOWN:
							{
								char key = event.key.keysym.sym;
								int vkey = chip8_keyboard_map (&chip8.keyboard, key);
								if (vkey != -1)
									{
										chip8_keyboard_down (&chip8.keyboard, vkey);
									}
							}
							break;

						case SDL_KEYUP:
							{
								char key = event.key.keysym.sym;
								int vkey = chip8_keyboard_map (&chip8.keyboard, key);
								if (vkey != -1)
									{
										chip8_keyboard_up (&chip8.keyboard, vkey);
									}
							}
							break;
						}
				}

			SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
			SDL_RenderClear (renderer);
			SDL_SetRenderDrawColor (renderer, 255, 255, 255, 0);

			for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++)
				{
					for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++)
						{
							if (chip8_screen_is_set (&chip8.screen, x, y))
								{
									SDL_Rect r = { 0 };
									r.x = x * EMULATOR_WINDOW_MULTIPLIER;
									r.y = y * EMULATOR_WINDOW_MULTIPLIER;
									r.w = EMULATOR_WINDOW_MULTIPLIER;
									r.h = EMULATOR_WINDOW_MULTIPLIER;
									SDL_RenderFillRect (renderer, &r);
								}
						}
				}

			SDL_RenderPresent (renderer);

			if (chip8.registers.DT > 0)
				{
					usleep (10 * 1000); // TODO: Fix this lmao
					chip8.registers.DT -= 1;
				}

			if (chip8.registers.ST > 0)
				{
					/* TODO: Beep */
					chip8.registers.ST -= 1;
				}

			unsigned short opcode
					= chip8_memory_get_short (&chip8.memory, chip8.registers.PC);
			chip8.registers.PC += 2;
			chip8_exec (&chip8, opcode);
		}

out:
	SDL_DestroyWindow (window);
	return EXIT_SUCCESS;
}
