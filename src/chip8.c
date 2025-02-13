#include "chip8.h"

#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

const char chip8_default_character_set[] = {
	0xf0, 0x90, 0x90, 0x90, 0xf0, // 1
	0x20, 0x60, 0x20, 0x20, 0x70, // 2
	0xf0, 0x10, 0xf0, 0x80, 0xf0, // 3
	0xf0, 0x10, 0xf0, 0x10, 0xf0, // 4
	0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
	0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
	0xf0, 0x10, 0x20, 0x40, 0x40, // 7
	0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
	0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
	0xf0, 0x90, 0xf0, 0x90, 0x90, // A
	0xe0, 0x90, 0xe0, 0x90, 0xe0, // B
	0xf0, 0x80, 0x80, 0x80, 0xf0, // C
	0xe0, 0x90, 0x90, 0x90, 0xe0, // D
	0xf0, 0x80, 0xf0, 0x80, 0xf0, // E
	0xf0, 0x80, 0xf0, 0x80, 0x80	// F
};

void
chip8_init (struct chip8 *chip8)
{
	memset (chip8, 0, sizeof (struct chip8));
	memcpy (&chip8->memory.memory, chip8_default_character_set,
					sizeof (chip8_default_character_set));
}

void
chip8_load (struct chip8 *chip8, const char *buf, size_t size)
{
	assert (size + CHIP8_PROGRAM_LOAD_ADDRESS < CHIP8_MEMORY_SIZE);
	memcpy (&chip8->memory.memory[CHIP8_PROGRAM_LOAD_ADDRESS], buf, size);
	chip8->registers.PC = CHIP8_PROGRAM_LOAD_ADDRESS;
}

static void
chip8_exec_extended_eight (struct chip8 *chip8, unsigned short opcode)
{
	unsigned char x = (opcode >> 8) & 0x000f;
	unsigned char y = (opcode >> 4) & 0x000f;
	unsigned char option = opcode & 0x000f;
	unsigned short tmp = 0;

	switch (option)
		{
			// 8xy0 - LD Vx, Vy. Vx = Vy
		case 0x00:
			chip8->registers.V[x] = chip8->registers.V[y];
			break;

			// 8xy1 - OR Vx, Vy. ORs Vx and Vy, stores the result in Vx
		case 0x01:
			chip8->registers.V[x] = chip8->registers.V[x] | chip8->registers.V[y];
			break;

			// 8xy2 - AND Vx, Vy. ANDs Vx and Vy, stores the result in Vx
		case 0x02:
			chip8->registers.V[x] = chip8->registers.V[x] & chip8->registers.V[y];
			break;

			// 8xy3 - XOR Vx, Vy. XORs Vx and Vy, stores the result in Vx
		case 0x03:
			chip8->registers.V[x] = chip8->registers.V[x] ^ chip8->registers.V[y];
			break;

			// 8xy4 - ADD Vx, Vy. Set Vx = Vx + Vy. Set VF = carry
		case 0x04:
			tmp = chip8->registers.V[x] + chip8->registers.V[y];
			chip8->registers.V[0x0f] = tmp > 0xff;
			chip8->registers.V[x] = tmp;
			break;

			// 8xy5 - SUB Vx, Vy. Set Vx = Vx - Vy, set VF = Not borrow
		case 0x05:
			chip8->registers.V[0x0f] = chip8->registers.V[x] > chip8->registers.V[y];
			chip8->registers.V[x] = chip8->registers.V[x] - chip8->registers.V[y];
			break;

			// 8xy6 - SHR Vx {, Vy}
		case 0x06:
			chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x01;
			chip8->registers.V[x] = chip8->registers.V[x] / 2;
			break;

			// 8xy7 - SUBN Vx, Vy. Sets Vx = Vy - Vx
		case 0x07:
			chip8->registers.V[0x0f] = chip8->registers.V[y] > chip8->registers.V[x];
			chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
			break;

			// 0xyE - SHL Vx {, Vy}
		case 0x0E:
			chip8->registers.V[0x0f] = chip8->registers.V[x] & 0b10000000;
			chip8->registers.V[x] = chip8->registers.V[x] * 2;
			break;
		}
}

static char
chip8_wait_for_keypress (struct chip8 *chip8)
{
	SDL_Event event;
	while (SDL_WaitEvent (&event))
		{
			if (event.type != SDL_KEYDOWN)
				continue;

			char c = event.key.keysym.sym;
			char chip8_key = chip8_keyboard_map (&chip8->keyboard, c);
			if (chip8_key != -1)
				return chip8_key;
		}

	return -1;
}

static void
chip8_exec_extended_F (struct chip8 *chip8, unsigned short opcode)
{
	unsigned char x = (opcode >> 8) & 0x000f;

	switch (opcode & 0x00ff)
		{
			// fx07 - LD Vx, DT. Set Vx to the delay timer value
		case 0x07:
			chip8->registers.V[x] = chip8->registers.DT;
			break;

			// fx0a - LD Vx, K.
		case 0x0A:
			{
				char key = chip8_wait_for_keypress (chip8);
				chip8->registers.V[x] = key;
			}
			break;

			// fx15 - LD DT, Vx. Set the delay timer to Vx
		case 0x15:
			chip8->registers.DT = chip8->registers.V[x];
			break;

			// fx18 - LD ST, Vx. Set the sound timer to Vx
		case 0x18:
			chip8->registers.ST = chip8->registers.V[x];
			break;

			// fx1e - Add I, Vx
		case 0x1e:
			chip8->registers.I = chip8->registers.I + chip8->registers.V[x];
			break;

			// fx29 - LD F, Vx
		case 0x29:
			chip8->registers.I = chip8->registers.V[x] * CHIP8_SPRITE_DEFAULT_HEIGHT;
			break;

			// fx33 - LD B, Vx
		case 0x33:
			{
				unsigned char hundreds = chip8->registers.V[x] / 100;
				unsigned char tens = (chip8->registers.V[x] / 10) % 10;
				unsigned char units = chip8->registers.V[x] % 10;
				chip8_memory_set (&chip8->memory, chip8->registers.I, hundreds);
				chip8_memory_set (&chip8->memory, chip8->registers.I + 1, tens);
				chip8_memory_set (&chip8->memory, chip8->registers.I + 2, units);
			}
			break;

			// fx55 - LD [I], Vx
		case 0x55:
			for (int i = 0; i <= x; i++)
				{
					chip8_memory_set (&chip8->memory, chip8->registers.I + i,
														chip8->registers.V[i]);
				}
			break;

			// fx65 - LD Vx, [I]
		case 0x65:
			for (int i = 0; i <= x; i++)
				{
					chip8->registers.V[i]
							= chip8_memory_get (&chip8->memory, chip8->registers.I + i);
				}
			break;
		}
}

static void
chip8_exec_extended (struct chip8 *chip8, unsigned short opcode)
{
	unsigned short nnn = opcode & 0x0fff;
	unsigned char x = (opcode >> 8) & 0x000f;
	unsigned char y = (opcode >> 4) & 0x000f;
	unsigned char kk = opcode & 0x00ff;
	unsigned char n = opcode & 0x000f;

	switch (opcode & 0xf000)
		{
			// jp addr, 1nnn jump to location nnn
		case 0x1000:
			chip8->registers.PC = nnn;
			break;

			// call addr, 2nnn call subroutine at location nnn
		case 0x2000:
			chip8_stack_push (chip8, chip8->registers.PC);
			chip8->registers.PC = nnn;
			break;

			// SE Vx, byte - 3xkk skip next instruction if Vx=kk
		case 0x3000:
			if (chip8->registers.V[x] == kk)
				chip8->registers.PC += 2;
			break;

			// SNE Vx, byte - 4xkk skip next instruction if Vx!=kk
		case 0x4000:
			if (chip8->registers.V[x] != kk)
				chip8->registers.PC += 2;
			break;

			// 5xy0 - SE Vx, Vy, skip next instruction if Vx=Vy
		case 0x5000:
			if (chip8->registers.V[x] == chip8->registers.V[y])
				chip8->registers.PC += 2;
			break;

			// 6xkk LD vx, byte - Vx = kk
		case 0x6000:
			chip8->registers.V[x] = kk;
			break;

			// 7xkk - ADD Vc, byte. Set Vx = Vx + kk
		case 0x7000:
			chip8->registers.V[x] += kk;
			break;

			// this is a more complex opcode, check the function
		case 0x8000:
			chip8_exec_extended_eight (chip8, opcode);
			break;

			// 9xy0 - SNE Vx, Vy. Skips next insturction if Vx != Vy
		case 0x9000:
			if (chip8->registers.V[x] != chip8->registers.V[y])
				chip8->registers.PC += 2;
			break;

			// Annn - LD I, addr. Sets the I register to nnn
		case 0xA000:
			chip8->registers.I = nnn;
			break;

			// bnnn - Jump to location nnn + V0
		case 0xB000:
			chip8->registers.PC = nnn + chip8->registers.V[0x00];
			break;

			// Cxkk - RND Vx, byte. Set Vx = random byte & kk
		case 0xC000:
			srand (clock ());
			chip8->registers.V[x] = (rand () % 255) & kk;
			break;

			// Dxyn - DRW Vx, Vy, nibble.
		case 0xD000:
			{
				const char *sprite
						= (const char *)&chip8->memory.memory[chip8->registers.I];
				chip8->registers.V[0x0f]
						= chip8_screen_draw_sprite (&chip8->screen, chip8->registers.V[x],
																				chip8->registers.V[y], sprite, n);
			}
			break;

			// keyboard operations
		case 0xE000:
			{
				switch (opcode & 0x00ff)
					{
						// Ex9e - SKP Vx, skip the next instruction if the key with value
						// of Vx is pressed
					case 0x9e:
						if (chip8_keyboard_is_down (&chip8->keyboard,
																				chip8->registers.V[x]))
							{
								chip8->registers.PC += 2;
							}
						break;

						// Exa1 - SKNP Vx, skip the next insturction if the key with value
						// of Vx is not pressed
					case 0xa1:
						if (!chip8_keyboard_is_down (&chip8->keyboard,
																				 chip8->registers.V[x]))
							{
								chip8->registers.PC += 2;
							}
						break;
					}
			}
			break;

		case 0xF000:
			chip8_exec_extended_F (chip8, opcode);
			break;
		}
}

void
chip8_exec (struct chip8 *chip8, unsigned short opcode)
{
	switch (opcode)
		{
			// clears the screen
		case 0x00E0:
			chip8_screen_clear (&chip8->screen);
			break;

			// returns from subroutine
		case 0x00EE:
			chip8->registers.PC = chip8_stack_pop (chip8);
			break;

		default:
			chip8_exec_extended (chip8, opcode);
			break;
		}
}
