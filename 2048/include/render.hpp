#ifndef RENDER_HPP
#define RENDER_HPP

#include <unordered_map>
#include <iostream>
#include <string>


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "game.hpp"
#include "SDL_gfxPrimitives.h"


namespace Render {
	extern char *buffer;
	extern std::unordered_map<int, SDL_Color> colors;

	void centerRect(SDL_Rect* rect, int parentWidth, int parentHeight);

	void renderGrid(SDL_Renderer* renderer, SDL_Rect gridRect, std::vector<Game::Cell> cells, int gridSize);
}

#endif