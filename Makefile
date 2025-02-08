CC=gcc
ECHO=echo -e
CFLAGS=-Wall -Werror -std=gnu99 -O0 -g -Iinclude
LIBS=-lSDL2

FILES=build/main.o
OUT=bin/chip8.out

all: $(FILES)
	@$(ECHO) "CC\t\t"$(FILES)
	@$(CC) $(CFLAGS) $(FILES) -o $(OUT) $(LIBS)

build/main.o: src/main.c
	@$(ECHO) "CC\t\t"$<
	@$(CC) $(CFLAGS) $< -c -o $@ $(LIBS)

run: all
	@$(ECHO) "Runing the Chip8 compiler"
	@$(OUT)

clean:
	@$(ECHO) "Cleaning files..."
	@rm -f $(FILES) $(OUT)
	@$(ECHO) "Cleaning complete..."
