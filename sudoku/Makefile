CC := gcc
CFLAGS :=  #-Wall -Wextra
SDFLAGS :=  -lSDL2  -lSDL2_ttf -lm

LIB := C:/msys64/mingw64/lib
INCLUDE := C:/msys64/mingw64/include

SRC := src
BIN := bin
EXE := sudoku

compile:
	@$(CC)  $(SRC)/*.c -o bin/$(EXE) -I include -I $(INCLUDE) -L $(LIB)  $(CFLAGS) $(SDFLAGS)

run:
	@bin/$(EXE)
	
debug:
	@gdb bin/$(EXE)

all:
	compile
	run
