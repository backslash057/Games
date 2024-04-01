CC := gcc
CFLAGS := -lm  #-Wall -Wextra
SDFLAGS := `sdl2-config --libs` -lSDL2_ttf 

SRC := src
BIN := bin
EXE := sudoku

compile:
	@$(CC)  $(SRC)/*.c -o bin/$(EXE) -I include  $(CFLAGS) $(SDFLAGS)

run:
	@bin/$(EXE)
	
all:
	compile
	run