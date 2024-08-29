CC := g++
CFLAGS := #-Wall -Wextra
LDFLAGS := -lSDL2 -lSDL2_ttf
EXE := bin/2048
SRC :=  src/main.cpp src/game.cpp src/render.cpp
HEADERS := include

compile:
	@$(CC) $(SRC) -o $(EXE) -I $(HEADERS) $(CFLAGS) $(LDFLAGS)
	
run: 
	@./$(EXE)

clean:
	@rm -f $(EXE)

.PHONY: compile run clean