#ifndef ROAD_H
#define ROAD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct road {
	int rectCount;
	SDL_Rect *rects;
	SDL_Texture* texture;

	int offset;
	int speed;
} Road;

Road initRoad(SDL_Renderer* renderer);

void updateRoad(Road *road, int windowHeight);

void renderRoad(SDL_Renderer* renderer, Road road, int windowWidth, int windowHeight, int roadWidth);


#endif