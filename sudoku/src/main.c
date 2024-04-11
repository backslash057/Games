#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gui.h"
#include "sudoku.h"

int main(int argc, char* argv[]) {
	int CELL = 60, LINE = 2;
	SDL_Point* selected = NULL;

	int selectedNum;
	int gridWidth = 9*CELL + 10*LINE;
	int gridHeight = gridWidth;
	int numGap = 10, numSize=65;
	int numGridWidth = 3*numSize + 2*numGap;
	int numGridHeight = numGridWidth;
	int btnWidth = 90, btnHeight = 40, btnGap = 20;
	int gap=40;

	SDL_Point origin = {80, 80};
	SDL_Rect gridBgRect = {origin.x, origin.y, gridWidth, gridHeight};
	SDL_Rect numGridRect = {origin.x + gridWidth + gap, origin.y + gridHeight - numGridHeight - btnHeight - btnGap, numGridWidth, numGridHeight};
	SDL_Rect eraseBtnRect = {origin.x + gridWidth + gap, origin.y + gridHeight - btnHeight, btnWidth, btnHeight};
	SDL_Rect solveBtnRect = {origin.x + gridWidth + gap + numGridWidth - btnWidth, origin.y + gridHeight - btnHeight, btnWidth, btnHeight};
	SDL_Rect timeRect = {origin.x + gridWidth + gap + numGridWidth - btnWidth, origin.y, btnWidth, btnHeight};
	SDL_Event event;

	SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
	
	SDL_Window* window = SDL_CreateWindow(
        "Sudoku", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		origin.x*2 + gridWidth + 30 + numGridWidth,
		gridHeight + origin.x*2, SDL_WINDOW_SHOWN
	);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	Cell grid[9][9];
	NumButton numGrid[9];

	generateGrid(grid);
	int *unfilled = unfill(grid, 40);

	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			SDL_Rect temp = {numGridRect.x + j*(numSize + numGap), numGridRect.y + i*(numSize + numGap), numSize, numSize};
			numGrid[i*3 + j].rect = temp;
			numGrid[i*3 + j].num = i*3 + j + 1;
			numGrid[i*3 + j].left = unfilled[i*3 + j];
		}
	}
	

	int time, initialTime = SDL_GetTicks64();
	int running = 1;
	while(running) {
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) {
				running = 0;
			}
			else if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button ==1) {
				if(collides(gridBgRect, event.button.x, event.button.y)) {
					if(selected == NULL) selected = malloc(sizeof(SDL_Point));
					selected->x = (event.button.x - origin.x - LINE) / (CELL + LINE);
					selected->y = (event.button.y - origin.y - LINE) / (CELL + LINE);
				}
				else if(collides(numGridRect, event.button.x, event.button.y)) {
					if((selected != NULL &&
						(event.button.x - numGridRect.x) % (numSize + numGap) < numSize && 
					    (event.button.y - numGridRect.y) % (numSize + numGap) < numSize)){

						selectedNum = (event.button.x-numGridRect.x)/(numSize+numGap)+(event.button.y-numGridRect.y)/(numSize+numGap)*3+1;
						if(numGrid[selectedNum].left != 0) {
							int result = play(grid, selected->x, selected->y, selectedNum);
							if(result > 0) numGrid[result-1].left++;
							if(result >= 0) numGrid[selectedNum-1].left--;
						}
					}
				}
				else if(collides(solveBtnRect, event.button.x, event.button.y)) {
					printf("Solve\n");
				}
				else if(collides(eraseBtnRect, event.button.x, event.button.y)) {
					if(selected != NULL && grid[selected->x][selected->y].num != 0) {
						int output = erase(grid, selected->x, selected->y);
						if(output != -1) numGrid[grid[selected->x][selected->y].num - 1].left++;
					}
				}
			}
		}

		time = SDL_GetTicks64() - initialTime;

		SDL_SetRenderDrawColor(renderer, 238, 244, 249, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &gridBgRect);
		displayGrid(renderer, origin, CELL, LINE, selected, grid);
		for(int i = 0; i < 9; i++) drawNumButton(renderer, numGrid[i]);
		drawButton(renderer, eraseBtnRect, "Erase");
		drawButton(renderer, solveBtnRect, "Solve");
		drawTime(renderer, timeRect, time/1000);

		SDL_RenderPresent(renderer);
	}

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}