CC := gcc
CFLAGS := #-Wall -Wextra
LDFLAGS := -lSDL2 -lm
EXE := bin/tetris
SRC :=  src/main.c  #src/SDL_gfxPrimitives.c
HEADERS := include

compile:
	@$(CC) $(SRC) -o $(EXE) -I $(HEADERS) $(CFLAGS) $(LDFLAGS)
	
run: 
	@./$(EXE)

clean:
	@rm -f $(EXE)

.PHONY: compile run clean