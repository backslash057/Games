#include "render.h"

SDL_Texture* loadPattern(SDL_Renderer* renderer, SDL_Surface* pattern, int w, int h, float scale) {
	SDL_Surface* background = SDL_CreateRGBSurface(
		0, w*scale, h*scale, 32,
		0xff000000, 0x00ff0000,
		0x0000ff00, 0x000000ff
	);

	SDL_Rect rect = {0, 0, pattern->clip_rect.w, pattern->clip_rect.h};
	SDL_Rect dstRect;

	SDL_Point margin = {50, 50};

	int rows = h / (rect.h + margin.y) + 2;
	int cols = w / (rect.w + margin.x) + 2;

	for(int i=-2; i<rows; i++) {
		for(int j=0; j<cols; j++) {
			rect.x = j*(rect.h + margin.y);
			rect.y = i*(rect.w + margin.x);

			dstRect = rect;

			SDL_UpperBlit(pattern, NULL, background, &dstRect);
		}
	}
	

	SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, background);

	return bgTexture;
}

void updateBackgroundOffset(SDL_Point* offset, SDL_Point speed) {

}

void renderBackgroundTexture(SDL_Renderer* renderer, SDL_Texture* backgroundTexture, SDL_Point offset) {
	SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
}