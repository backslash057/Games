#include <stdlib.h>

#include "gui.h"
#include "sudoku.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

	int CELL = 50, LINE = 2;
	int width = 9*CELL + 10*LINE;
	int height = width;
	SDL_Point origin = {50, 50};

	int gridWidth = 200, gridHeight = 250;
	int numMargin = 10, numSize=60;

	SDL_Rect mainRect = {origin.x, origin.y, width, height};
	SDL_Point* selected = 0;

    SDL_Window* window = SDL_CreateWindow(
        "Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		origin.x*2 + width + 30 + gridWidth,
		height + origin.x*2, SDL_WINDOW_RESIZABLE
	);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_Rect gridRect = {origin.x + width + 20, origin.y + height - gridHeight, gridWidth, gridHeight};

	int btnWidth = 90, btnHeight = 40;
	SDL_Rect eraseBtnRect = {origin.x + width + 20, origin.y + height - btnHeight, btnWidth, btnHeight};
	SDL_Rect solveBtnRect = {origin.x + width + 20 + gridWidth - btnWidth, origin.y + height - btnHeight, btnWidth, btnHeight};
	SDL_Rect timeRect = {origin.x + width + 20 + gridWidth - btnWidth, origin.y, btnWidth, btnHeight};
	
	SDL_Event event;

	Cell cells[9][9];
	for(int i=0; i<9; i++){
		for(int j=0; j<9; j++){
			cells[i][j].num = 1;
			cells[i][j].valid = 1;
		}
	}

	int running = 1;
	while(running) {
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) {
				running = 0;
			}
			else if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button ==1) {
				if((origin.x < event.button.x && event.button.x < origin.x + width &&
					origin.y < event.button.y && event.button.y < origin.y + height)) {
					
					SDL_Point coords;
					coords.x = (event.button.x - origin.x - LINE) / (CELL + LINE);
					coords.y = (event.button.y - origin.y - LINE) / (CELL + LINE);

					selected = &coords;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 238, 244, 249, 255);
		SDL_RenderClear(renderer);
		
		/* Fill background */
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &mainRect);


		/* Draw lines*/
		displayGrid(renderer, origin, CELL, LINE, selected, cells);

		/* Display the UI */
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				SDL_Rect numRect = {gridRect.x + j*(numSize + numMargin), gridRect.y + i*(numSize + numMargin), numSize, numSize};
				drawNumButton(renderer, numRect, i*3+j+1, 9);
			}
		}

		drawButton(renderer, eraseBtnRect, "Erase");
		drawButton(renderer, solveBtnRect, "Solve");

		/* Display the renderer on screen*/
		SDL_RenderPresent(renderer);
	}

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}