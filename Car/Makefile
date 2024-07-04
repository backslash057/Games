CC := gcc
CFLAGS := #-Wall -Wextra
LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf
SRC := main.c player.c enemy.c road.c utils.c
EXE := car


compile:
	@$(CC) $(SRC) -o $(EXE) $(LDFLAGS) $(CFLAGS)


run:
	@./$(EXE)

.PHONY: compile run