CC := gcc
CFLAGS := $(shell sdl2-config --cflags) #-Wall #-Wextra
LDFLAGS := $(shell sdl2-config --libs)
SRCS := src/main.c src/game.c
EXE := bin/wordle
HEADERS := include

compile:
	@$(CC) -o $(EXE) $(SRCS) -I include $(CFLAGS) $(LDFLAGS)

run:
	@./$(EXE)

clean:
	@rm -f bin/*

.PHONY: compile run clean