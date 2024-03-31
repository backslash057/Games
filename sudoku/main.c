#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SDL_gfxPrimitives.h"

void drawNumButton(SDL_Renderer* renderer, SDL_Rect rect, int main, int sec) {
    Uint32 bg = 0xFBFCFDFF;
    Uint32 border = 0x3B3B3BFF;
    SDL_Color fg =  {59, 59, 59, 255};

    SDL_Surface *surface = SDL_CreateRGBSurface(
        0, rect.w, rect.h, 32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
    );
    roundedRectangleColor(surface, 0, 0, rect.w-1, rect.h-1, 3, border);
    roundedBoxColor(surface, 1, 1, rect.w-2, rect.h-2, 3, bg);

    TTF_Font* mainFont = TTF_OpenFont("Arial.ttf", 30);
    TTF_Font* secFont = TTF_OpenFont("Arial.ttf", 18);

	char mainChar[] = {main + '0', '\0'};
	char secChar[] = {sec + '0', '\0'};
    SDL_Surface* mainTextSurface = TTF_RenderText_Blended(mainFont, mainChar, fg);
    SDL_Surface* secTextSurface = TTF_RenderText_Blended(secFont, secChar, fg);

    SDL_Rect temp = {surface->w/2-mainTextSurface->w/2, surface->h/2-mainTextSurface->h/2, mainTextSurface->w, mainTextSurface->h};
    SDL_BlitSurface(mainTextSurface, NULL, surface, &temp);

    SDL_Rect temp2 = {surface->w-secTextSurface->w-3, surface->h-secTextSurface->h-3, secTextSurface->w, secTextSurface->h};
    SDL_BlitSurface(secTextSurface, NULL, surface, &temp2);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
	SDL_FreeSurface(mainTextSurface);
    SDL_FreeSurface(secTextSurface);
	TTF_CloseFont(mainFont);
	TTF_CloseFont(secFont);
}

void drawButton(SDL_Renderer* renderer, SDL_Rect rect, char* text)
{
    SDL_Surface *surface = SDL_CreateRGBSurface(
        0, rect.w, rect.h, 32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
    );
    roundedRectangleColor(surface, 0, 0, rect.w-1, rect.h-1, 3, 0xCCD0D4FF);
    roundedBoxColor(surface, 1, 1, rect.w-2, rect.h-2, 3, 0xFBFCFDFF);
    

    TTF_Font* font = TTF_OpenFont("Arial.ttf", 20);
    SDL_Color color = {59, 59, 59, 0};

    SDL_Surface *textsurface = TTF_RenderText_Blended(font, text, color);

    SDL_Rect destRect;
    destRect.x = surface->w/2 - textsurface->w/2;
    destRect.y = surface->h/2 - textsurface->h/2;
    destRect.w = textsurface->w;
    destRect.h = textsurface->h;
    
    SDL_BlitSurface(textsurface, NULL, surface, &destRect);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(textsurface);
	TTF_CloseFont(font);
}

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
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				SDL_Rect numRect = {gridRect.x + j*(numSize + numMargin), gridRect.y + i*(numSize + numMargin), numSize, numSize};
				drawNumButton(renderer, numRect, i*3+j+1, 6);
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