#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "gui.h"


void drawLines(SDL_Renderer* renderer, SDL_Point origin, int CELL, int LINE, int width, int height) {
	SDL_SetRenderDrawColor(renderer, 206, 206, 206, 255);
	for(int i=0; i<3; i++) {
		SDL_Rect VLine1Rect = {origin.x + i*3*(CELL + LINE) + (CELL+LINE), origin.y, LINE, height};
		SDL_Rect VLine2Rect = {VLine1Rect.x + (CELL + LINE), origin.y, LINE, height};
		SDL_RenderFillRect(renderer, &VLine1Rect);
		SDL_RenderFillRect(renderer, &VLine2Rect);

		SDL_Rect HLine1Rect = {origin.x, origin.y + i*3*(CELL + LINE)+ (LINE + CELL), width, LINE};
		SDL_Rect HLine2Rect = {origin.x, HLine1Rect.y + (CELL+LINE), width, LINE};
		SDL_RenderFillRect(renderer, &HLine1Rect);
		SDL_RenderFillRect(renderer, &HLine2Rect);
	}

	SDL_SetRenderDrawColor(renderer, 61, 112, 220, 255);
	for(int i=0; i<4; i++) {
		SDL_Rect VLineRect = {origin.x + 3*i*(CELL + LINE), origin.y, LINE, height};
		SDL_Rect HLineRect = {origin.x , origin.y+ 3*i*(CELL + LINE), width, LINE};

		SDL_RenderFillRect(renderer, &VLineRect);
		SDL_RenderFillRect(renderer, &HLineRect);
	}
}

int test(int argc, char *argv[]) {
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

	SDL_Window* window = SDL_CreateWindow("Sudoku",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		origin.x*2 + width + 30 + gridWidth,
		height + origin.x*2,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	SDL_Renderer* renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED
	);

	SDL_Rect gridRect = {
		origin.x + width + 20,
		origin.y + height - gridHeight,
		gridWidth,
		gridHeight
	};

	NumButton numGrid[9];
	// for (int i=0; i<3; i++) {
	// 	for (int j=0; j<3; j++) {
	// 		SDL_Rect numRect = {
	// 			gridRect.x + i*(numSize + numMargin),
	// 			gridRect.y + j*(numSize + numMargin),
	// 			numSize,
	// 			numSize
	// 		};

	// 		numGrid[i*3 + j]= CreateButton(renderer, numRect, 0xFBFCFDFF, 0xCCD0D4FF);
	// 	}
	// }

	int btnWidth = 90, btnHeight = 40;
	SDL_Rect eraseBtnRect = {
		origin.x + width + 20,
		origin.y + height - btnHeight,
		btnWidth,
		btnHeight
	};

	SDL_Rect solveBtnRect = {
		origin.x + width + 20 + gridWidth - btnWidth,
		origin.y + height - btnHeight,
		btnWidth,
		btnHeight
	};

	SDL_Rect timeRect = {
		origin.x + width + 20 + gridWidth - btnWidth,
		origin.y,
		btnWidth,
		btnHeight
	};
	

	Button eraseBtn = CreateButton(renderer, eraseBtnRect, "Erase", 0x000000FF, 0x000000FF, 0xCCD0D4FF, 1);
	Button solveBtn = CreateButton(renderer, solveBtnRect, "Solve", 0x000000FF, 0xFBFCFDFF, 0xCCD0D4FF, 1);

	SDL_Event event;
	

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

		/* Fll the cell, line, row and block if a cell is selected */
		if(selected != NULL){
			
			SDL_SetRenderDrawColor(renderer, 237, 247, 255, 255);

			SDL_Rect cellRect = {origin.x + selected->x*(CELL + LINE) + LINE, origin.y + selected->y*(CELL + LINE) + LINE, CELL, CELL};

			SDL_Rect rowRect = {origin.x + LINE, origin.y + selected->y*(CELL + LINE)+LINE, width-LINE, CELL};
			SDL_Rect colRect = {origin.x + selected->x*(CELL + LINE) + LINE , origin.y + LINE, CELL, height-LINE};

			SDL_Point BStart = {selected->x - selected->x%3, selected->y - selected->y%3};
			SDL_Rect blockRect = {origin.x + BStart.x*(LINE+CELL)+LINE, origin.y + BStart.y*(CELL+LINE)+LINE, 2*LINE+3*CELL, 2*LINE+3*CELL};

			SDL_RenderFillRect(renderer, &rowRect);
			SDL_RenderFillRect(renderer, &colRect);
			SDL_RenderFillRect(renderer, &blockRect);
			
			SDL_SetRenderDrawColor(renderer, 197, 207, 255, 255);
			SDL_RenderFillRect(renderer, &cellRect);
		}

		/* Draw lines*/
		drawLines(renderer, origin, CELL, LINE, width, height);

		/* Display the UI */
		// for (int k = 0; k < 9; k++) drawButton(renderer, numGrid[k]);
		drawButton(renderer, eraseBtn);
		drawButton(renderer, solveBtn);


		/* Display the renderer on screen*/
		SDL_RenderPresent(renderer);
	}

	SDL_Quit();
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	
	return 0;
}