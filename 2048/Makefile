CC := g++
CFLAGS := #-Wall -Wextra
LDFLAGS := -lSDL2 -lm -lSDL2_gfx
EXE := bin/2048
SRC :=  src/main.cpp  #src/SDL_gfxPrimitives.c
HEADERS := include

compile:
	@$(CC) $(SRC) -o $(EXE) -I $(HEADERS) $(CFLAGS) $(LDFLAGS)
	
run: 
	@./$(EXE)

clean:
	@rm -f $(EXE)

.PHONY: compile run clean