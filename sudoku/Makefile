CC := gcc
CFLAGS :=  #-Wall -Wextra
SDFLAGS := `sdl2-config --libs` -lSDL2_ttf -lm #-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

SRCS := main.c SDL_gfxPrimitives.c
EXE := sudoku

compile:
	@$(CC) $(SRCS) -o $(EXE) $(CFLAGS) $(SDFLAGS)

run:
	@./$(EXE)

	
all:
	compile
	run