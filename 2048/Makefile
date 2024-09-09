CC := g++
CFLAGS := #-Wall -Wextra
LDFLAGS := -lSDL2 -lSDL2_ttf
EXE := bin/2048
SRC :=  src/*.cpp #src/SDL_gfxPrimitives.c
HEADERS := include

compile:
	@$(CC) $(SRC) -o $(EXE) -I $(HEADERS) $(CFLAGS) $(LDFLAGS)
	
test:
	@$(CC) test.cpp -o test
	@./test

run: 
	@./$(EXE)

clean:
	@rm -f $(EXE)

.PHONY: compile run clean test
