CC := gcc
CFLAGS := $(sdl2-config --cflags) #-Wall -Wextra
LDFLAGS := -lSDL2 -lSDL2_image -lm
EXE := bin/mine
SRC :=  src/main.c
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