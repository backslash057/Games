#include <SDL2/SDL.h>


int main(int argc, char *argv[]) {
	int width=1000, height=800;


	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow(
		"Tetris",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_RESIZABLE 
	);


	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, -1);
	SDL_Event event;


	int running=1;
	while(running) {
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) running=0;
		}


		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);


		SDL_RenderPresent(renderer);
	}


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}