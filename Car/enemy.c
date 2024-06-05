#include "enemy.h"


int totalEnemies = 8;
char* filenames[8] = {"vehicles/Ambulance.png", "vehicles/Black_viper.png", "vehicles/Car.png",
					"vehicles/Mini_truck.png", "vehicles/Mini_van.png", "vehicles/Police.png",
					"vehicles/taxi.png", "vehicles/truck.png"
};

float enemySpeeds[8] = {2, 2, 2, 2, 2, 3, 2, 2}; // initialiser toute les vitesses a 2%

SDL_Texture* enemyTextures[8];
SDL_Rect enemyRects[8];


Enemy* initEnemies(SDL_Renderer* renderer) {
	SDL_Surface* enemySurface;

	for(int i=0; i<totalEnemies; i++) {
		enemySurface = IMG_Load(filenames[i]);

		enemyTextures[i] = SDL_CreateTextureFromSurface(renderer, enemySurface);
		enemyRects[i] = enemySurface->clip_rect;

		SDL_FreeSurface(enemySurface);
	}
}

void generateNewEnemy(Enemy *enemies, int* enemiesCount) {
	Enemy enemy;
	enemy.x = rand()%4 * 33.33;
	enemy.y = 0;

	int id = rand()%totalEnemies;
	enemy.speed = enemySpeeds[id];
	enemy.w = enemyRects[id].w;
	enemy.h = enemyRects[id].h;
	enemy.texture = enemyTextures[id];

	for(int i=*enemiesCount; i>0; i--) enemies[i] = enemies[i-1];
	enemies[0] = enemy;

	int count = 1;
	for(int i=1; i < *enemiesCount+1; i++) {
		if(enemies[i].y < 150) count++;
	}

	*enemiesCount = count;
}

void updateEnemies(Enemy *enemies, int enemiesCount, float speedFactor) {
	for(int i=0; i<enemiesCount; i++) {
		enemies[i].y += enemies[i].speed * speedFactor;
	}
}

void renderEnemies(SDL_Renderer* renderer, Enemy* enemies, int enemiesCount, int windowHeight, int navigableRoadWidth, int navigableRoadOrigin) {
	for(int i=0; i<enemiesCount; i++) {
		int enemyWidth = navigableRoadWidth/5;
		int enemyHeight = enemyWidth*enemies[i].h/enemies[i].w;

		SDL_Rect enemyDstRect = {
			navigableRoadOrigin + enemies[i].x*navigableRoadWidth/100,
			enemies[i].y * windowHeight/100 - enemyHeight,
			enemyWidth,
			enemyHeight
		};

		SDL_RenderCopy(renderer, enemies[i].texture, NULL, &enemyDstRect);
	}
}