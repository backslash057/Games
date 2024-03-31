#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "snake.h"
#include "texture.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MENU_HEIGHT 80

// Path to the asset folder ending with '/'
char *ASSETS_PATH = "assets/";

void render(SDL_Renderer *renderer, int xOrigin, int yOrigin, Snake snake, Fruit fruit)
{
    SDL_Rect rect;
    Block *current = snake.head;
    char path[100];

    // rendering the fruit
    SDL_Surface *imageSurface = IMG_Load("assets/apple.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_Rect fruitRect = {fruit.x*BLOCK_SIZE+xOrigin, fruit.y*BLOCK_SIZE+yOrigin, BLOCK_SIZE, BLOCK_SIZE};

    SDL_RenderCopy(renderer, texture, NULL, &fruitRect);

    
    rect.w = BLOCK_SIZE;
    rect.h = BLOCK_SIZE;

    // rendering the snake
    int i = 0;
    while (current->next != NULL)
    {
        strcpy(path, ASSETS_PATH);

        rect.x = xOrigin;
        rect.y = yOrigin;

        // Snake head
        if (i == 0)
        {
            rect.x += current->x * BLOCK_SIZE;
            rect.y += current->y * BLOCK_SIZE;

            strcat(path, headTexturePath(snake.dir));
            i++;
        }
        else
        {
            rect.x += current->next->x * BLOCK_SIZE;
            rect.y += current->next->y * BLOCK_SIZE;

            // middle body
            if (current->next->next != NULL)
            {
                Vector2 vect1 = {current->x - current->next->x, current->y - current->next->y};
                Vector2 vect2 = {current->next->next->x - current->next->x, current->next->next->y - current->next->y};
                strcat(path, bodyTexturePath(vect1, vect2));
            }
            // snake tail
            else
            {
                Vector2 vect = {current->x - current->next->x, current->y - current->next->y};
                strcat(path, tailTexturePath(vect));
            }

            current = current->next;
        }

        SDL_Surface *image_surface = IMG_Load(path);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image_surface);

        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
}

int main(int argc, char *args[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_InitError: %s", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow(
        "Simple SDL Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        xSize*BLOCK_SIZE+200, ySize*BLOCK_SIZE+200,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == NULL)
    {
        printf("SDL_CreateWindowError: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("SDL_CreateRendererError: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("Erreur lors de l'initialisation de SDL_image : %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    // Initialisation de SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf n'a pas pu être initialisé ! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    /*
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer n'a pas pu être initialisé ! Mix_Error: %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }
    */

    int isDead = 0;
    int score = 0;
    Vector2 dir = {1, 0};
    Snake snake = snake_(5, 5, dir, 4);
    Fruit fruit = fruit_(snake);
    int quit = 0;
    SDL_Event e;
    int wWidth;
    int wHeight;
    int xOrigin;
    int yOrigin;

    SDL_Rect rect;
    SDL_Rect scoreRect = {0, 0, 0, 0};
    SDL_Rect barRect = {0, 0, xSize*BLOCK_SIZE, MENU_HEIGHT};
    SDL_Rect bgRect = {0, 0, xSize*BLOCK_SIZE, ySize*BLOCK_SIZE};

    SDL_Surface* bgImage = IMG_Load("assets/background.jpg");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bgImage);
        
    SDL_Color scoreColor = {255, 255, 255, 255};
    
    TTF_Font* scoreFont = TTF_OpenFont("font/Arial.ttf", 24);  // Remplacez "arial.ttf" par le chemin de votre police

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(scoreFont, "Score: ", scoreColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    
    // Exemple de débogage
    if (scoreFont == NULL) {
        printf("La police n'a pas pu être chargée ! TTF_Error: %s\n", TTF_GetError());
        // ... (autres messages de débogage)
        return 1;
    }

    if (scoreSurface == NULL) {
        printf("La surface de texte n'a pas pu être créée ! TTF_Error: %s\n", TTF_GetError());
        // ... (autres messages de débogage)
        return 1;
    }

    if (scoreTexture == NULL) {
        printf("La texture de texte n'a pas pu être créée ! SDL_Error: %s\n", SDL_GetError());
        // ... (autres messages de débogage)
        return 1;
    }
    
    /*
    Mix_Music *eatSound = Mix_LoadMUS("sounds/eat.mp3");
    Mix_Music *hitSound = Mix_LoadMUS("sounds/die.mp3");
    */

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = 1;
            if (e.type == SDL_KEYDOWN && !isDead)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_LEFT:
                    dir.x = -1;
                    dir.y = 0;
                    break;
                case SDLK_RIGHT:
                    dir.x = 1;
                    dir.y = 0;
                    break;
                case SDLK_UP:
                    dir.x = 0;
                    dir.y = -1;
                    break;
                case SDLK_DOWN:
                    dir.x = 0;
                    dir.y = 1;
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 87, 138, 52, 255);
        SDL_RenderClear(renderer);

        SDL_GetRendererOutputSize(renderer, &wWidth, &wHeight);
        xOrigin = (wWidth - bgRect.w)/2;
        yOrigin = (wHeight - bgRect.h - MENU_HEIGHT)/2;

        bgRect.x = xOrigin;
        bgRect.y = yOrigin + MENU_HEIGHT;

        barRect.x = xOrigin;
        barRect.y = yOrigin;
        scoreRect.x = xOrigin;
        scoreRect.y = yOrigin;

        SDL_SetRenderDrawColor(renderer, 74, 117, 44, 255);
        SDL_RenderFillRect(renderer, &barRect);

        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

        if (isAboutToDie(snake, dir))
        {
            // Mix_PlayMusic(hitSound, 1);
            isDead = 1;
        }
        else if (isAboutToEat(snake, fruit))
        {
            // Mix_PlayMusic(eatSound, 1);
            fruit = fruit_(snake); // the fruit is moved

            grow(&snake, dir);

            score++;
        }
        else move(&snake, dir);


        SDL_RenderCopy(renderer, texture, NULL, &bgRect);

        render(renderer, xOrigin, yOrigin+MENU_HEIGHT, snake, fruit);
        SDL_RenderPresent(renderer);

        if(!isDead) SDL_Delay(150);
        else SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    // Mix_FreeMusic(eatSound);
    // Mix_FreeMusic(hitSound);

    // Mix_CloseAudio();
    SDL_Quit();

    return 0;
}