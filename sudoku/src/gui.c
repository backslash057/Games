#include "gui.h"
#include "sudoku.h"
#include <stdio.h>

void drawNumButton(SDL_Renderer* renderer, SDL_Rect rect, int main, int sec) {
    Uint32 bg = 0xFBFCFDFF;
    Uint32 border = 0x3B3B3BFF;
    SDL_Color fg =  {59, 59, 59, 255};
    SDL_Color secFg =  {159, 159, 159, 255};
    
    SDL_Surface *surface = SDL_CreateRGBSurface(
        0, rect.w, rect.h, 32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
    );
    roundedRectangleColor(surface, 0, 0, rect.w-1, rect.h-1, 3, border);
    roundedBoxColor(surface, 1, 1, rect.w-2, rect.h-2, 3, bg);

    TTF_Font* mainFont = TTF_OpenFont("assets/fonts/Arial.ttf", 30);
    TTF_Font* secFont = TTF_OpenFont("assets/fonts/calibri-leger.ttf", 18);

	char mainChar[] = {main + '0', '\0'};
	char secChar[] = {sec + '0', '\0'};
    SDL_Surface* mainTextSurface = TTF_RenderText_Blended(mainFont, mainChar, fg);
    SDL_Surface* secTextSurface = TTF_RenderText_Blended(secFont, secChar, secFg);

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
    
    TTF_Font* font = TTF_OpenFont("assets/fonts/Arial.ttf", 20);
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

void displayGrid(SDL_Renderer* renderer, SDL_Point origin, int CELL, int LINE, SDL_Point* selected, Cell cells[9][9]) {
    int width = 9*CELL + 10*LINE;
	int height = width;
    
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
    
    SDL_Surface* surface = SDL_CreateRGBSurface(
        0, width, height, 32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
    );

    TTF_Font* font = TTF_OpenFont("assets/fonts/Arial.ttf", 35);

    SDL_Color normal = {0, 0, 0, 255};
    SDL_Color invalid = {250, 100, 100, 255};

    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            if(cells[i][j].num == 0) continue;

            char num[2] = {cells[i][j].num + '0', '\0'};
            SDL_Surface* number = (cells[i][j].valid)? TTF_RenderText_Blended(font, num, normal) : TTF_RenderText_Blended(font, num, invalid);
            
            SDL_Rect rect = {LINE+i*(LINE+CELL) + (CELL/2-number->w/2), LINE+j*(CELL+LINE) + (CELL/2-number->h/2), CELL, CELL};

            SDL_UpperBlit(number, NULL, surface, &rect);
            SDL_FreeSurface(number);
        }
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstRect = {origin.x, origin.y, width, height};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}