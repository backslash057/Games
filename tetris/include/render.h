#pragma once


#include <SDL2/SDL.h>

#include <stdio.h>

typedef struct {

} Button;

typedef struct {
    Button buttons[3];
} Menu;

SDL_Texture* loadBackgroundPattern(SDL_Renderer* renderer, int w, int h, float ratio);

void updateBackgroundOffset(SDL_Point* offset, SDL_Point speed);

void renderBackgroundTexture(SDL_Renderer* renderer, SDL_Texture* backgroundTexture, SDL_Point offset);
