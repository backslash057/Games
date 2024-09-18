#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "game.h"
#include "render.h"

int main(int argc, char *argv[]) {
	int width=1000, height=800;


	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);


	SDL_Window* window = SDL_CreateWindow(
		"Tetris",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_RESIZABLE 
	);


	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, -1);
	SDL_Event event;

	SDL_Surface* pattern = IMG_Load("assets/pattern.png");

	SDL_Point offset = {0, 0};
	float scale = 0.8;
	SDL_Texture* backgroundTexture = loadPattern(renderer, pattern, width, height, scale);

	int running=1;
	while(running) {
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) running=0;
			else if(event.type == SDL_WINDOWEVENT && event.window.event  == SDL_WINDOWEVENT_RESIZED) {
				int w=event.window.data1, h=event.window.data2;

				SDL_DestroyTexture(backgroundTexture);
				backgroundTexture = loadPattern(renderer, pattern, w, h, scale);
			}
		}

		
		SDL_SetRenderDrawColor(renderer, 104, 115, 146, 255);
		SDL_RenderClear(renderer);

		renderBackgroundTexture(renderer, backgroundTexture, offset);
		

		SDL_RenderPresent(renderer);

		SDL_Delay(16);
	}


	SDL_FreeSurface(pattern);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}