#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int test2(int argc, char *argv[]) {
	

	SDL_Window* window = SDL_CreateWindow("Sudoku",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		300,
        300,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	SDL_Renderer* renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED
	);

    SDL_Event event;
	

	int running = 1;
	while(running) {
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) {
				running = 0;
			}
		}

		SDL_SetRenderDrawColor(renderer, 238, 244, 249, 255);
		SDL_RenderClear(renderer);
		

        TTF_Font* font = TTF_OpenFont("arial.ttf", 12);
        SDL_Color color = {0, 0, 0, 255};

        SDL_Surface* textSurface = TTF_RenderText_Blended(font, "Salut", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect rect = {0, 0, 100, 100};
        SDL_RenderCopy(renderer, texture, NULL, &rect);

		/* Display the renderer on screen*/
		SDL_RenderPresent(renderer);
	}

	SDL_Quit();
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	
	return 0;
}