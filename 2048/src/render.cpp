#include "render.hpp"

std::unordered_map<int, SDL_Color> Render::colors = {
    {2,     {0xfc, 0xe9, 0x4f, 0xff}},  // Yellowish
    {4,     {0x8a, 0xe2, 0x34, 0xff}},  // Greenish
    {8,     {0xfc, 0xaf, 0x3e, 0xff}},  // Orange
    {16,    {0x72, 0x9f, 0xcf, 0xff}},  // Light blue
    {32,    {0xad, 0x7f, 0xa8, 0xff}},  // Light purple
    {64,    {0xc1, 0x7d, 0x11, 0xff}},  // Brown
    {128,   {0xef, 0x29, 0x29, 0xff}},  // Red
    {256,   {0xc4, 0xa0, 0x00, 0xff}},  // Dark yellow
    {1024,  {0xce, 0x5c, 0x00, 0xff}},  // Dark orange
    {2048,  {0x00, 0x00, 0x00, 0xff}}   // Black
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
