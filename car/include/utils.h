#ifndef UTILS_H
#define UTILS_H

#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "player.h"
#include "enemy.h"

void loadGraphics(SDL_Renderer* renderer);

void renderGameOverMenu(SDL_Renderer* renderer, int windowWidth, int windowHeight);

void renderStartMenu(SDL_Renderer* renderer, int windowWidth, int windowHeight);

void renderPauseMenu(SDL_Renderer* renderer, int windowWidth, int windowHeight);

int checkEvent(int x, int y, int menuId);

int playerIsDead(Player player, Enemy *enemies, int enemiesCount);

void renderScore(SDL_Renderer* renderer, int playerScore, int windowWidth);



#endif