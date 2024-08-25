#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

std::unordered_map<int, Uint32> colors = {
    {2, 0xfce94f},
    {4, 0x8ae234},
    {8, 0xfcaf3e},
    {16, 0x729fcf},
    {32, 0xad7fa8},
    {64, 0xc17d11},
    {128, 0xef2929},
    {256, 0xc4a000},
    {1024, 0xce5c00},
    {2048, 0x000000} 
};

void renderGrid(SDL_Renderer* renderer, int *grid, SDL_Rect gridRect, int rowCount) {
    int margin = 5;
    int width = (gridRect.w - margin*(rowCount+1)) / rowCount;
    
    SDL_Point origin;

    for(int i=0; i<rowCount; i++) {
        for(int j=0; j<rowCount; j++) {
            origin.x = j*(margin + width) + margin + gridRect.x;
            origin.y = i*(margin + width) + margin + gridRect.y;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            // SDL_RenderFillRect(renderer, &rect);
            roundedBoxColor(renderer,
                origin.x, origin.y,
                origin.x+width, origin.y+width,
                4,
                0x00000000
            );
        }
    }
}


void centerRect(SDL_Rect* rect, int parentWidth, int parentHeight) {
    rect->x = (parentWidth - rect->w) / 2;
    rect->y = (parentHeight - rect->h) / 2;
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    SDL_CreateWindowAndRenderer(
        800, 650,
        SDL_WINDOW_RESIZABLE,
        &window, &renderer
    );

    SDL_Rect gridRect = {100, 25, 600, 600};
    int windowWidth, windowHeight;


    int rowCount = 5;
    int grid[rowCount*rowCount] = {2};
    

    bool running = true;
    while(running) {
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) running = false;
        }

        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        centerRect(&gridRect, windowWidth, windowHeight);


        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 186, 186, 186, 255);
        SDL_RenderFillRect(renderer, &gridRect);

        roundedBoxColor(renderer,
            200, 200, 400, 400,
            4,
            0x000000ff
        );

        renderGrid(renderer, grid, gridRect, rowCount);

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}