#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <time.h>
#include <stdlib.h>
#include <math.h>

typedef struct block {
	int revealed; // 1 if the block is reveald, 0 otherwise
	int data; // the value behind the block (-1 for mines)
} Block;


int collides(SDL_Rect rect, int x, int y) {
	return rect.x <= x <= rect.x+rect.w && rect.y <= y <= rect.y + rect.h;
}

void generateGrid(Block **grid, int m, int n, int originX, int originY, int count) {
	for(int i=0; i<m; i++) {
		for(int j=0; j<n; j++) {
			grid[i][j].data = 0; // initialise with empty blocks
			grid[i][j].revealed = 0; // initialise block to unrevealed
		}
	}

	int size = m*n;
	int vect[size];
	for(int i=0; i<size; i++) vect[i] = i;

	// shuffle the vector
	srand(time(NULL));
	int j, temp;
	for(int i=0; i<size-1; i++) {
		j = rand()%(size-1);
		temp = vect[i];
		vect[i] = vect[j];
		vect[j] = temp;
	}

	int i=0, c=0, x, y;
	while(c < count)  {
		x = vect[i]/n, y = vect[i]%n;

		if((abs(originX - x)>1 && abs(originY - y)>1)) {
			grid[x][y].data = -1;
			for(int i=-1; i<=1; i++) {
				for(int j=-1; j<=1; j++) {
					if((i==0 && j==0) || x+i<0 || y+j<0 || x+i>=m || y+j>=n) continue;
					if(grid[x+i][y+j].data != -1) grid[x+i][y+j].data++;
				}
			}
			c++;
		}
		i++;
	}
}


int main(int argc, char *argv[]) {
	int width=1100, height=900, barHeight=60, btnWidth=150, btnHeight=40;
	int m=9, n=9, x, y;
	char* difficulty = "Medium";

	SDL_Rect barRect = {0, 0, width, barHeight};
	SDL_Rect DifficultyBtnRect = {10, barHeight/2-btnHeight/2, btnWidth, btnHeight};
	SDL_Rect dropBtnRect = {10 + btnWidth, barHeight/2-btnHeight/2, btnHeight, btnHeight};
	SDL_Rect helpBtnRect = {10, barHeight + 10, 45, 45};
	SDL_Rect gridRect = {width/2 - m*32, (height+barHeight)/2-n*32, m*64, n*64};

	// SDL_Init(SDL_INIT_VIDEO);
	// IMG_Init(IMG_INIT_PNG);
	SDL_Window* window = SDL_CreateWindow(
		"Minesweeper",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN
	);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, -1);
	SDL_Event event;

	SDL_Surface* dropSurface = IMG_Load("assets/down-chevron.png");
	SDL_Surface* helpSurface = IMG_Load("assets/question_.png");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, dropSurface);
	SDL_Texture* helpTexture = SDL_CreateTextureFromSurface(renderer, helpSurface);

	Block **grid;
	grid = malloc(sizeof(Block*)*m);
	for(int i=0; i<m; i++) grid[i] = (Block*) malloc(sizeof(Block)*n);
	// generateGrid(grid, m, n, 4, 4, 10);

	SDL_Surface* surface = SDL_CreateRGBSurface(
		0, m*64, n*64, 32,
		0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF 
	);

	SDL_Surface* tileSurface = IMG_Load("assets/tile.png");

	SDL_Surface *numsSurface[5];
	char* path = malloc(sizeof(char)*13);
	for(int i=0; i<5; i++) {
		sprintf(path, "assets/%d.png", i);
		numsSurface[i] = IMG_Load(path);
	}


	SDL_Surface* emptySurface = IMG_Load("assets/0.png");


	int generated=0;
	int running = 1;
	while(running) {
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) running=0;
			else if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
				if(collides(gridRect, event.button.x, event.button.y)) {
					x = (event.button.x-gridRect.x)/64, y = (event.button.y-gridRect.y)/64;
					if(!generated) {
						generated =1;
						generateGrid(grid, m, n, x, y, 10);
					}
					else {
						
					}
					grid[x][y].revealed = 1;
				}
			}
		}

		for(int i=0; i<n; i++) {
			for(int j=0; j<n; j++) {
				printf("%d ", grid[i][j].data);
				// SDL_Rect rect = {i*64, j*64, 64, 64};

				// if(generated && grid[i][j].revealed && grid[i][j].data >=0) {
				// 	SDL_BlitSurface(numsSurface[grid[i][j].data], NULL, surface, &rect);
				// }
				// else SDL_BlitSurface(tileSurface, NULL, surface, &rect);
			}
			printf("\n");
		}

		
		SDL_Texture* gridTexture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_SetRenderDrawColor(renderer, 50, 98, 168, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 41, 47, 133, 255);
		SDL_RenderFillRect(renderer, &barRect);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &DifficultyBtnRect);
		SDL_RenderFillRect(renderer, &dropBtnRect);

		SDL_RenderCopy(renderer, texture, NULL, &dropBtnRect);
		SDL_RenderCopy(renderer, helpTexture, NULL, &helpBtnRect);

		SDL_RenderCopy(renderer, gridTexture, NULL, &gridRect);

		SDL_RenderPresent(renderer);
	}

	return 0;
}