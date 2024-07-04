#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <time.h>
#include <stdlib.h>

typedef struct enemy {
	int x, y;
	int w, h;

	float speed;

	SDL_Texture* texture;
} Enemy;


Enemy* initEnemies(SDL_Renderer* renderer);

void generateNewEnemy(Enemy *enemies, int* enemiesCount);

void updateEnemies(Enemy *enemies, int enemiesCount, float speedFactor);

void renderEnemies(SDL_Renderer* renderer, Enemy* enemies, int enemiesCount, int windowHeight, int navigableRoadWidth, int navigableRoadOrigin);

#endif