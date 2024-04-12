#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


int main(int argc, char *argv[]) {
	int width=1000, height=800, barHeight=60, btnWidth=150, btnHeight=40;
	char* difficulty = "Medium";
	SDL_Rect barRect = {0, 0, width, barHeight};
	SDL_Rect DifficultyBtnRect = {10, barHeight/2-btnHeight/2, btnWidth, btnHeight};
	SDL_Rect dropBtnRect = {10 + btnWidth, barHeight/2-btnHeight/2, btnHeight, btnHeight};
	SDL_Rect helpBtnRect = {0, barHeight + 10, 40, 40};

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	SDL_Window* window = SDL_CreateWindow(
		"Minesweeper",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN
	);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, -1);
	SDL_Event event;

	SDL_Surface* dropSurface = IMG_Load("assets/down-chevron.png");
	SDL_Surface* helpSurface = IMG_Load("assets/question.png");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, dropSurface);
	SDL_Texture* helpTexture = SDL_CreateTextureFromSurface(renderer, helpSurface);

	printf("%d %d %s\n", dropSurface->clip_rect.w, dropSurface->clip_rect.h, SDL_GetError());

	int running = 1;
	while(running) {
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) running=0;
		}

		SDL_SetRenderDrawColor(renderer, 50, 98, 168, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 41, 47, 133, 255);
		SDL_RenderFillRect(renderer, &barRect);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &DifficultyBtnRect);
		SDL_RenderFillRect(renderer, &dropBtnRect);

		SDL_RenderCopy(renderer, texture, NULL, &dropBtnRect);
		SDL_RenderCopy(renderer, texture, NULL, &helpBtnRect);

		SDL_RenderPresent(renderer);
	}


	return 0;
}