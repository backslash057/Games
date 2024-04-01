#include "gui.h"

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

    TTF_Font* mainFont = TTF_OpenFont("assets/fonts/Arial.ttf", 30);
    TTF_Font* secFont = TTF_OpenFont("assets/fonts/Arial.ttf", 18);

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