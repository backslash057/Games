#ifndef GUI_H
#define GUI_H

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SDL_gfxPrimitives.h"
#include "sudoku.h"

void drawNumButton(SDL_Renderer* renderer, SDL_Rect rect, int main, int sec);

void drawButton(SDL_Renderer* renderer, SDL_Rect rect, char* text);

void displayGrid(SDL_Renderer* renderer, SDL_Point origin, int CELL, int LINE, SDL_Point* selected, Cell cells[9][9]);




#endif