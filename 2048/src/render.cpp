#include "render.hpp"

std::unordered_map<int, Uint32> Render::colors = {
        {2,     0xfce94f},
        {4,     0x8ae234},
        {8,     0xfcaf3e},
        {16,    0x729fcf},
        {32,    0xad7fa8},
        {64,    0xc17d11},
        {128,   0xef2929},
        {256,   0xc4a000},
        {1024,  0xce5c00},
        {2048,  0x000000} 
};

char* Render::buffer = (char*) malloc(sizeof(char) * 6);

void Render::centerRect(SDL_Rect* rect, int parentWidth, int parentHeight) {
    rect->x = (parentWidth - rect->w) / 2;
    rect->y = (parentHeight - rect->h) / 2;
}

void Render::renderGrid(SDL_Renderer* renderer, SDL_Rect gridRect, std::vector<Game::Cell> cells) {
    int margin = 5;
    int width = (gridRect.w - margin*(gridSize+1)) / gridSize;
    
    SDL_Rect cellRect;
    cellRect.w = cellRect.h = width;

    for(int i=0; i<gridSize; i++) {
        for(int j=0; j<gridSize; j++) {
            cellRect.x = j*(margin + width) + margin + gridRect.x;
            cellRect.y = i*(margin + width) + margin + gridRect.y;

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &cellRect);
        }
    }

    SDL_Color textColor = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("Ubuntu-Th.ttf", 134);

    for(Cell cell : cells) {
        if(cell.value == 0) continue;
        
        sprintf(buffer, "%d", cell.value);

        SDL_Surface* textSurface = TTF_RenderText_Blended(font, buffer, textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);

        cellRect.x = round(cell.animX * (margin + width) + margin + gridRect.x);
        cellRect.y = round(cell.animY * (margin + width) + margin + gridRect.y);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &cellRect);
        SDL_RenderCopy(renderer, texture, NULL, &cellRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(texture);
    }

    TTF_CloseFont(font);
}