#pragma once


#include <SDL2/SDL.h>

#include <stdio.h>


SDL_Texture* loadPattern(SDL_Renderer* renderer, SDL_Surface* pattern, int w, int h, float scale);

void updateBackgroundOffset(SDL_Point* offset, SDL_Point speed);

void renderBackgroundTexture(SDL_Renderer* renderer, SDL_Texture* backgroundTexture, SDL_Point offset);