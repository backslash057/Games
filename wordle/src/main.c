#include <SDL2/SDL.h>


int handleEvents() {
	SDL_Event e;

	while(SDL_PollEvent(&e) != 0) {
		if(e.type == SDL_QUIT) return 0;
	}

	return 1;
}

void center(SDL_Rect* rect, int w, int h) {
	rect->x = (w - rect->w)/2;
	rect->y = (h - rect->h)/2;
}

int main(int argc, char *argv[]) {
	int WIDTH=800, HEIGHT=900;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow(
		"Wordle",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HEIGHT,
		SDL_WINDOW_RESIZABLE
	);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	int w, h;

	int keyWidth = 48, keyHeight = 58;
	int keyboardColMargin = 6, keyboardRowMargin = 8;

	// Variables for letters grid
	int letterCount = 5; int turnCount = 6;
	int cellWidth = 80, gridMargin = 5; 

	SDL_Rect kbdRect = {0, 0, (keyWidth + keyboardRowMargin)*10, (keyHeight + keyboardRowMargin)*3};
	SDL_Rect gridRect = {0, 0, (cellWidth + gridMargin)*letterCount, (cellWidth + gridMargin)*turnCount};

	int running=1;
	while(running) {
		running = handleEvents();

		SDL_GetWindowSize(window, &w, &h);
		// printf("%d %d\n", w, h);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		kbdRect.x = (w - kbdRect.w) /2;
		kbdRect.y = h - kbdRect.h;

		gridRect.x = (w - gridRect.w) /2;
		gridRect.y = (h - kbdRect.h - gridRect.h)/2;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &gridRect);
		SDL_RenderDrawRect(renderer, &kbdRect);

		SDL_RenderPresent(renderer);
	}

	SDL_Quit();

	return 0;
}