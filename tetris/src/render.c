#include "render.h"

SDL_Texture* loadPattern(SDL_Renderer* renderer, int w, int h, float ratio) {
	SDL_Surface* background = SDL_CreateRGBSurface(
		0, w, h, 32,
		0xff000000, 0x00ff0000,
		0x0000ff00, 0x000000ff
	);

	SDL_Rect dstRect = {0, 0, pattern->clip_rect.w*ratio, pattern->clip_rect.h*ratio};
	SDL_Point margin = {50*ratio, 50*ratio};

	int rows = h / (dstRect.h + margin.y) + 1;
	int cols = w / (dstRect.w + margin.x) + 1;


	for(int i=-1; i<rows; i++) {
		for(int j=0; j<cols+1; j++) {
			dstRect.w = pattern->clip_rect.w * ratio;
			dstRect.h = pattern->clip_rect.h * ratio;
			dstRect.x = j*(dstRect.h + margin.y);
			dstRect.y = i*(dstRect.w + margin.x);

			SDL_UpperBlitScaled(pattern, NULL, background, &dstRect);
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

