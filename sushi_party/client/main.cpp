#include <ostream>
#include <istream>
#include <iostream>
#include <string>

#include <SDL2/SDL.h>

#include "network.hpp"
#include "snake.hpp"

#define HOST "127.0.0.1"
#define PORT 12345

char buffer[1000];

/*
void SDL_RenderCircle(SDL_Renderer *renderer, int centreX, int centreY, int rayon) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    const double PI = 3.14159265358979323846;
    const int nombrePoints = 2*rayon; // Augmentez le nombre pour une meilleure précision

    for (int i = 0; i < nombrePoints; i++) {
        double angle = PI * i / nombrePoints;
        int x = static_cast<int>(centreX + rayon * std::cos(angle));
        int y = static_cast<int>(centreY + rayon * std::sin(angle));

        SDL_RenderDrawLine(renderer, x, y, x, y+2*(centreY-y));
    }
}
*/

void SDL_RenderDrawCircle(SDL_Renderer *renderer, int32_t centerX, int32_t centerY, int32_t radius)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   while (x >= y)
   {
      SDL_RenderDrawLine(renderer, centerX+x, centerY+y, centerX-x, centerY+y);
      SDL_RenderDrawLine(renderer, centerX+y, centerY+x, centerX-y, centerY+x);
      SDL_RenderDrawLine(renderer, centerX+x, centerY-y, centerX-x, centerY-y);
      SDL_RenderDrawLine(renderer, centerX+y, centerY-x, centerX-y, centerY-x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

int main(int argc, char *argv[]) {
  Connection conn = Connection(HOST, PORT);

  if(conn.getStatus() == -1) {
    perror("Erreur d'etablissement de la connexion");
    return -1;
  }

  SDL_Window *window;
  SDL_Renderer *renderer;

  SDL_Event event;
  bool quit=false;
  int deltaTime, previousTime=0, time;

  Vector2 velocity, new_velocity;

  Snake snake = Snake(300, 300, 50);
  
  
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_CreateWindowAndRenderer(
    800, // WIDTH
    800, // HEIGHT
    SDL_WINDOW_RESIZABLE,
    &window,
    &renderer
  );

  while(!quit) {
    const Uint8 *etatTouches = SDL_GetKeyboardState(nullptr);

    
    velocity = Vector2(0, 0);
    velocity.x = (etatTouches[SDL_SCANCODE_LEFT]==1)? -1 : 0;
    velocity.x += (etatTouches[SDL_SCANCODE_RIGHT]==1)? 1: 0;

    velocity.y = (etatTouches[SDL_SCANCODE_UP]==1)? -1 : 0;
    velocity.y += (etatTouches[SDL_SCANCODE_DOWN]==1)? 1 : 0;

    
    if(velocity != Vector2(0, 0)) {
      snake.move(deltaTime, velocity);
      conn.send_(std::to_string(snake.x) + " " + std::to_string(snake.y));
    }

    // SDL events handling
    while(SDL_PollEvent(&event) != 0){
      if(event.type == SDL_QUIT) quit=true;
    }

    SDL_SetRenderDrawColor(renderer, 195, 229, 202, 255);
    SDL_RenderClear(renderer);

    time = SDL_GetTicks64();
    deltaTime = time - previousTime;
    previousTime = time;


    // rendering the snake
    SDL_RenderDrawCircle(renderer, snake.x, snake.y, snake.radius);

    SDL_RenderPresent(renderer);
    SDL_Delay(5);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  conn.close_();

  return 0;
}

