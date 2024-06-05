#include "player.h"


Player createPlayer(SDL_Renderer* renderer, int initialX, float initialSpeed) {
	Player player;

	player.x = initialX;
	player.y = 98;
	player.speed = initialSpeed;

	SDL_Surface* playerSurface = IMG_Load("vehicles/Audi.png");
	player.texture = SDL_CreateTextureFromSurface(renderer, playerSurface);
	player.w = playerSurface->clip_rect.w;
	player.h = playerSurface->clip_rect.h;

	SDL_FreeSurface(playerSurface);
	
	return player;
}

void movePlayer(Player *player, float direction) {
	player->x += player->speed * direction;
}


void renderPlayer(SDL_Renderer* renderer, Player player, int windowHeight, int navigableRoadWidth, int navigableRoadOrigin) {
	int playerWidth = navigableRoadWidth/5;
	int playerHeight = playerWidth*player.h/player.w;

	SDL_Rect playerDstRect = {
		navigableRoadOrigin + navigableRoadWidth*player.x/100,
		player.y * windowHeight/100 - playerHeight,
		playerWidth, playerHeight
	};

	SDL_RenderCopy(renderer, player.texture, NULL, &playerDstRect);
}

