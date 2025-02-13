CC=gcc
ECHO=echo -e
CFLAGS=-Wall -Werror -std=gnu99 -O0 -g -Iinclude
LIBS=-lSDL2

FILES=build/main.o build/mem.o build/stack.o build/keyboard.o build/chip8.o \
			build/screen.o
OUT=bin/chip8.out

all: $(FILES)
	@$(ECHO) "CC\t\t"$(FILES)
	@$(CC) $(CFLAGS) $(FILES) -o $(OUT) $(LIBS)

build/main.o: src/main.c
	@$(ECHO) "CC\t\t"$<
	@$(CC) $(CFLAGS) $< -c -o $@ $(LIBS)

build/mem.o: src/mem.c
	@$(ECHO) "CC\t\t"$<
	@$(CC) $(CFLAGS) $< -c -o $@ $(LIBS)

build/stack.o: src/stack.c
	@$(ECHO) "CC\t\t"$<
	@$(CC) $(CFLAGS) $< -c -o $@ $(LIBS)

build/keyboard.o: src/keyboard.c
	@$(ECHO) "CC\t\t"$<
	@$(CC) $(CFLAGS) $< -c -o $@ $(LIBS)

build/chip8.o: src/chip8.c
	@$(ECHO) "CC\t\t"$<
	@$(CC) $(CFLAGS) $< -c -o $@ $(LIBS)

build/screen.o: src/screen.c
	@$(ECHO) "CC\t\t"$<
	@$(CC) $(CFLAGS) $< -c -o $@ $(LIBS)

run: all
	@$(ECHO) "Runing the Chip8 compiler"
	@$(OUT) $(rom)

clean:
	@$(ECHO) "Cleaning files..."
	@rm -f $(FILES) $(OUT)
	@$(ECHO) "Cleaning complete..."
