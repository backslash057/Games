#ifndef GUI_H
#define GUI_H

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SDL_gfxPrimitives.h"
#include "sudoku.h"

typedef struct {
	SDL_Rect rect;

	int num;
	int left;
} NumButton;

void drawNumButton(SDL_Renderer* renderer, NumButton btn);

void drawButton(SDL_Renderer* renderer, SDL_Rect rect, char* text);

void displayGrid(SDL_Renderer* renderer, SDL_Point origin, int CELL, int LINE, SDL_Point* selected, Cell cells[9][9]);

int rectPointcollides(SDL_Rect rect, SDL_Point point);

int collides(SDL_Rect rect, int x, int y);

#endif