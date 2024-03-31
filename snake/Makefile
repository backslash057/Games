CC := gcc
CFLAGS := $(shell sdl2-config --cflags --libs) #-Wall -Wextra
LDFLAGS := $(shell sdl2-config --libs) #-lSDL2_image -lSDL2_ttf -lSDL2_mixer
EXE := game


all: compile run

compile:
	@$(CC) src/*.c -o bin/$(EXE) $(CFLAGS) $(LDFLAGS)

run:
	@./bin/$(EXE)

.PHONY: snake all