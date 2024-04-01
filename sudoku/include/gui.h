#ifndef GUI_H
#define GUI_H

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SDL_gfxPrimitives.h"

void drawNumButton(SDL_Renderer* renderer, SDL_Rect rect, int main, int sec);

void drawButton(SDL_Renderer* renderer, SDL_Rect rect, char* text);

void drawLines(SDL_Renderer* renderer, SDL_Point origin, int CELL, int LINE, int width, int height);



#endif