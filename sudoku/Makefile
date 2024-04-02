CC := gcc
CFLAGS :=  #-Wall -Wextra
SDFLAGS := -lmingw32 -lSDL2main -lSDL2 -Dmain=SDL_main -lSDL2_ttf

LIB := C:/msys64/mingw64/lib
INCLUDE := C:/msys64/mingw64/include

SRC := src
BIN := bin
EXE := sudoku

compile:
	@$(CC)  $(SRC)/*.c -o bin/$(EXE) -I include -I $(INCLUDE) -L $(LIB)  $(CFLAGS) $(SDFLAGS)

run:
	@bin/$(EXE)
	
all:
	compile
	run
