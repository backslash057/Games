CC := gcc
CFLAGS := #-Wall -Wextra
LDFLAGS := -lSDL2 -lSDL2_image -lm
EXE := bin/mine
SRC :=  src/main.c  #src/SDL_gfxPrimitives.c
HEADERS := include

compile:
	@$(CC) $(SRC) -o $(EXE) -I $(HEADERS) $(CFLAGS) $(LDFLAGS)

compdebug:
	@$(CC) -g $(SRC) -o $(EXE) -I $(HEADERS) $(CFLAGS) $(LDFLAGS)

run: 
	@./$(EXE)

clean:
	@rm -f $(EXE)

.PHONY: compile run clean