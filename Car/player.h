#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct player {
	int x, y;
	int w, h;

	float speed;

	SDL_Texture* texture;
} Player;

Player createPlayer(SDL_Renderer* renderer, int initialX, float initialSpeed);

void movePlayer(Player *player, float speed);

void renderPlayer(SDL_Renderer* renderer, Player player, int windowHeight, int navigableRoadWidth, int navigableRoadOrigin);

#endif