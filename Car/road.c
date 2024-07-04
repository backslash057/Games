#include "road.h"


Road initRoad(SDL_Renderer* renderer) {
	Road road;

	road.rectCount = 4;
	road.rects = malloc(sizeof(SDL_Rect)*road.rectCount);
	road.offset = 0;
	road.speed = 10;

	SDL_Surface* roadSurface = IMG_Load("assets/road.png");

	road.texture = SDL_CreateTextureFromSurface(renderer, roadSurface);
	SDL_FreeSurface(roadSurface);

	return road;
}

void updateRoad(Road *road, int windowHeight) {
	road->offset += road->speed * windowHeight/1000;
}


void renderRoad(SDL_Renderer* renderer, Road road, int windowWidth, int windowHeight, int roadWidth) {
	for(int i=0; i<3; i++) {
		SDL_Rect rect = {
			(windowWidth - roadWidth)/2,
			(road.offset - windowHeight*i + 2*windowHeight)%(3*windowHeight) - 2*windowHeight,
			roadWidth, windowHeight
		};
		road.rects[i] = rect;
	}
	for(int i=0; i<3; i++) SDL_RenderCopy(renderer, road.texture, NULL, &road.rects[i]);
	
}