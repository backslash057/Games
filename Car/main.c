#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "player.h"
#include "enemy.h"
#include "road.h"
#include "utils.h"

#define MENU 1
#define PLAYING 2

int WIDTH=400, HEIGHT=400;
int roadRatio = 840/650; // ratio largeur/hauteur de l'image de route
float navigableRoadRatio = (float)420/840; // pourcentage navigable de l'image de route 
float navigableOriginRatio = (float)174/840;

int main(int argc, char *argv[]) {
	// Initialisation de la graine de generation aleatoire
	srand(time(NULL));

	// Initialisation de SDL, SDL_Image et SDL_TTF
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	// Creation de la feentre et du renderer
	SDL_Window *window = SDL_CreateWindow(
		"Car game",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT,
		SDL_WINDOW_RESIZABLE
	);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, -1);
	SDL_Event event;

	// Variables d"etat de l'interface
	int depth = MENU;
	int currentTime;
	int lastSpawnTime = SDL_GetTicks64();
	int windowWidth, windowHeight;
	int enemySpawnRate;
	float speedFactor; // Facteur vitesse pour variation de difficulté
	int playerScore = 0;

	Road road = initRoad(renderer);

	Player player = createPlayer(renderer, 50, 2);
	int isDead = 0;
	int isPaused = 0;

	initEnemies(renderer);
	Enemy *enemies = malloc(sizeof(Enemy) * 50);
	int enemiesCount = 0;

	loadGraphics(renderer);

	// Boucle prinipale
	int running = 1;
	while(running) {
		if(depth == PLAYING && !isDead) {
			const Uint8 *state = SDL_GetKeyboardState(NULL);

			if (state[SDL_SCANCODE_LEFT]) {
				movePlayer(&player, -1);
				if(player.x < 0) player.x = 0;
			}
			if (state[SDL_SCANCODE_RIGHT]) {
				movePlayer(&player, 1);
				if(player.x > 100) player.x = 100;
			}
		}

		// Verification des evenements
		while(SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_QUIT) running = 0;

			else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
				if(depth == PLAYING && !isDead) isPaused = (isPaused)? 0: 1;
			}
			else if(event.type == SDL_MOUSEBUTTONDOWN) {

				if(depth==MENU || (depth==PLAYING && isDead==1) || isPaused) {
					int menuId;
					if(depth == MENU) menuId=0;
					else if (isDead) menuId=1;
					else menuId = 2;
					int buttonId = checkEvent(event.button.x, event.button.y, menuId);

					if(buttonId != -1) {
						switch(buttonId) {
						case 0: // Nouvelle partie facile
							enemySpawnRate = 1500;
							road.speed = 8;
							speedFactor = 1;
							depth = PLAYING;
							break;
						case 1: // Nouvelle partie moyen
							enemySpawnRate = 1300;
							road.speed = 14;
							speedFactor = 1.2;
							depth = PLAYING;
							break;
						case 2: // Nouvelle partie difficile
							enemySpawnRate = 1100;
							road.speed = 20;
							speedFactor = 1.5;
							depth = PLAYING;
							break;
						case 3: // Retour au menu
							playerScore=0;
							isDead = 0;
							isPaused = 0;
							road.offset = 0;
							player.x = 50;
							enemiesCount = 0;
							depth = MENU;
							break;
						case 4: // Relancer la partie
							playerScore=0;
							isDead = 0;
							road.offset = 0;
							player.x = 50;
							enemiesCount = 0;
							break;
						case 5: // continuer
							isPaused = 0;
						default: break;
						}
					}
				}
			}
		}

		// Actualisation du rendu graphique
		SDL_SetRenderDrawColor(renderer, 102, 102, 102, 255);
		SDL_RenderClear(renderer);

		SDL_GetWindowSize(window, &windowWidth, &windowHeight);

		// Profondeur 0: On affiche uniquement le menu
		if(depth == MENU) {
			// Affichage du menu ici
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(renderer, NULL);
			renderStartMenu(renderer, windowWidth, windowHeight);
		}

		// Profondeur 1: On affiche le jeu
		else if(depth == PLAYING) {
			int roadWidth = windowHeight * roadRatio;
			int navigableRoadWidth = roadWidth*navigableRoadRatio;
			int navigableRoadOrigin = (windowWidth - roadWidth)/2 + roadWidth*navigableOriginRatio;

			// Generer un nouveau ennemi
			currentTime = SDL_GetTicks64();
			if(currentTime - lastSpawnTime > enemySpawnRate) {
				lastSpawnTime = currentTime;

				if(!isPaused) generateNewEnemy(enemies, &enemiesCount);
			}

			// Afficher les differentes couches de la route
			renderRoad(renderer, road, windowWidth, windowHeight, roadWidth);
		
			// Afficher les ennemies
			renderEnemies(renderer, enemies, enemiesCount, windowHeight, navigableRoadWidth, navigableRoadOrigin);

			// Afficher le joueur
			renderPlayer(renderer, player, windowHeight, navigableRoadWidth, navigableRoadOrigin);

			renderScore(renderer, playerScore, windowWidth);

			// Verification de la mort du joueur
			if(isDead || playerIsDead(player, enemies, enemiesCount)) {
				isDead = 1;

				renderGameOverMenu(renderer, windowWidth, windowHeight);
			}
			else if(isPaused) {
				renderPauseMenu(renderer, windowWidth, windowHeight);
			}
			else {
				playerScore += 1;
				// Mettre a jour le mouvement de la route
				updateRoad(&road, windowHeight);

				// Mettre ajour la position des nouveaux ennemies
				updateEnemies(enemies, enemiesCount, speedFactor);	
			}
		}
		
		// Standardisation des FPS
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}