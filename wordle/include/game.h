#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

typedef enum {
	CURRENTLINE,	/* On current line but is not on cursor */
	CURSOR, 		/* Directly editable by the player  */
	ONPLACE, 		/* Discovered and the letter is well placed */
	WRONGPLACED,	/* Discovered and the wrongly placed */
	NOTFOUND,		/* Discovered but not in the word */
	UNDISCOVERED	/* Not discovered yet */
} CellState;

typedef struct cell_ {
	int status;
	char letter;
} Cell;

typedef struct grid_ {
	int rowCount; /* m */
	int colCount; /* n */

	Cell** cells;
} Grid;

typedef struct Keyboard_ {
	int rowCount;
	int *colCounts;

	SDL_Rect *rects; 
} Keyboard;


Keyboard keyboard();
renderKeyboard(SDL_Renderer* renderer, Keyboard keyboard);

Grid grid();
renderGrid(SDL_Renderer* renderer, Grid grid);

#endif