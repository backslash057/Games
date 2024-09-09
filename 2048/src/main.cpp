#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector>

#include "game.hpp"
#include "render.hpp"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    Game::initRng();

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

    int animSteps = 0;
    bool animationEnded;
    int gridSize = 4;
    std::vector<Game::Cell> cells;

    Game::addRandomCell(cells, gridSize);
    Game::addRandomCell(cells, gridSize);

    bool running = true;
    while(running) {
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) running = false;
            else if(event.type == SDL_KEYDOWN) {
                if(animSteps == 0) { // all animations are terminated
                    if(event.key.keysym.sym == SDLK_LEFT) {
                        Game::shiftLeft(cells, gridSize);
                        animSteps = 10;
                    }
                    else if(event.key.keysym.sym == SDLK_UP) {
                        Game::shiftUp(cells, gridSize);
                        animSteps = 10;
                    }
                    else if(event.key.keysym.sym == SDLK_RIGHT) {
                        Game::shiftRight(cells, gridSize);
                        animSteps = 10;
                    }
                    else if(event.key.keysym.sym == SDLK_DOWN) {
                        Game::shiftDown(cells, gridSize);
                        animSteps = 10;
                    }
                }
            }
        }

        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        Render::centerRect(&gridRect, windowWidth, windowHeight);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // SDL_SetRenderDrawColor(renderer, 186, 186, 186, 255);
        // SDL_RenderFillRect(renderer, &gridRect);

        animationEnded = (animSteps == 1);

        Game::update(cells, animSteps);

        if(animationEnded) {
            stack(cells);
            addRandomCell(cells, gridSize);
        }

        Render::renderGrid(renderer, gridRect, cells, gridSize);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }


    return 0;
}