CC := gcc
CFLAGS := #-Wall -Wextra
LDFLAGS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lm -lSDL2_image -lSDL2_ttf -lSDL2_mixer
EXE := game


all: compile run

compile:
	@$(CC) src/*.c -o bin/$(EXE) $(CFLAGS) $(LDFLAGS)

run:
	@./bin/$(EXE)

.PHONY: snake all