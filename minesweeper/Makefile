CC := gcc
CFLAGS := #-Wall -Wextra
LDFLAGS := -lSDL2 -lSDL2_image -lm
EXE := bin/mine
SOURCES := test.c src/main.c #src/SDL_gfxPrimitives.c
HEADERS := include

compile:
	@$(CC) $(SOURCES) -o $(EXE) -I $(HEADERS) $(CFLAGS) $(LDFLAGS)


run:
	@./$(EXE)