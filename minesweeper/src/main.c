#include <SDL2/SDL.h>

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();





	int running = 1;
	while(running) {
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) running = 0;
		}

		SDL_RenderPresent();
	}



	return 0;
}