CC := g++
CFLAGS := #-Wall -Wextra
LDFLAGS := `sdl2-config --libs` #-lSDL2_image -lSDL2_ttf -lSDL2_mixer
EXE := game

all: compile run

compile:
	@$(CC) *.cpp -o $(EXE) $(CFLAGS) $(LDFLAGS)

run:
	@./$(EXE)

.PHONY: snake all