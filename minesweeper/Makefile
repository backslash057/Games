CC := gcc
CFLAGS := #-Wall -Wextra
LDFLAGS := -lSDL2 -lSDL2_image -lm
EXE := bin/mine
SRC :=  src/*.c
HEADERS := include

compile:
	@$(CC) $(SRC) -o $(EXE) -I $(HEADERS) $(CFLAGS) $(LDFLAGS)

run: 
	@./$(EXE)

clean:
	@rm -f $(EXE)

.PHONY: compile run clean