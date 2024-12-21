#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "game.h"
#include "render.h"

#include "menu.h"

typedef enum {
	MENU = 0x0,
	GAME = 0x1
} Display;



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


	SDL_Point backgroundOffset = {0, 0};
	float ratio = 0.5;
	SDL_Texture* backgroundTexture = loadPattern(renderer, width, height, ratio);

	SDL_Rect windowRect = {0, 0, width, height};

	SDL_Rect menuRect = {0, 0, 600, 600};

	int running=1;
	while(running) {
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) running=0;
			else if(event.type == SDL_WINDOWEVENT && event.window.event  == SDL_WINDOWEVENT_RESIZED) {
				int w=event.window.data1, h=event.window.data2;

				SDL_DestroyTexture(backgroundTexture);
				backgroundTexture = loadPattern(renderer, w, h, ratio);
			}
		}

		SDL_GetWindowSize(window, &windowRect.w, &windowRect.h);

		// center the menu buttons

		SDL_SetRenderDrawColor(renderer, 104, 115, 146, 255);
		SDL_RenderClear(renderer);


		renderBackgroundTexture(renderer, backgroundTexture, backgroundOffset);
		
		renderMenu(renderer, windowRect, menu);

		SDL_RenderPresent(renderer);

		SDL_Delay(16);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}