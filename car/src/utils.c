#include "utils.h"

SDL_Texture* startMenuTexture;
SDL_Texture* easyBtnTexture;
SDL_Texture* mediumBtnTexture;
SDL_Texture* hardBtnTexture;

SDL_Texture* gameOverMenuTexture;
SDL_Texture* menuBtnTexture;
SDL_Texture* restartBtnTexture;

SDL_Texture* pauseMenuBgTexture;
SDL_Texture* continueBtnTexture;

SDL_Rect easyBtnRect;
SDL_Rect mediumBtnRect;
SDL_Rect hardBtnRect;

SDL_Rect menuBtnRect;
SDL_Rect restartBtnRect;

SDL_Rect continueBtnRect;
SDL_Rect menuPauseBtnRect;


TTF_Font* scoreFont;

SDL_Rect rect(int x, int y, int w, int h) {
	SDL_Rect Rect = {x, y, w, h};
	return Rect;
}

int collides(SDL_Rect rect, int x, int y) {
	return (rect.x < x && x<rect.x+rect.w && rect.y < y && y<rect.y+rect.h);
}

void loadGraphics(SDL_Renderer* renderer) {
	SDL_Surface* startMenuSurface = IMG_Load("assets/game-menu-bg.png");
	startMenuTexture = SDL_CreateTextureFromSurface(renderer, startMenuSurface);

	SDL_Surface* easyBtnSurface = IMG_Load("assets/easyButton.png");
	easyBtnTexture = SDL_CreateTextureFromSurface(renderer, easyBtnSurface);

	SDL_Surface* mediumBtnSurface = IMG_Load("assets/mediumButton.png");
	mediumBtnTexture = SDL_CreateTextureFromSurface(renderer, mediumBtnSurface);

	SDL_Surface* hardBtnSurface = IMG_Load("assets/hardButton.png");
	hardBtnTexture = SDL_CreateTextureFromSurface(renderer, hardBtnSurface);


	SDL_Surface* menuBgSurface = IMG_Load("assets/game-over-bg.png");
	gameOverMenuTexture = SDL_CreateTextureFromSurface(renderer, menuBgSurface);

	SDL_Surface* menuBtnSurface = IMG_Load("assets/menuBtn.png");
	menuBtnTexture = SDL_CreateTextureFromSurface(renderer, menuBtnSurface);

	SDL_Surface* restartBtnSurface = IMG_Load("assets/restartBtn.png");
	restartBtnTexture = SDL_CreateTextureFromSurface(renderer, restartBtnSurface);


	SDL_Surface* pauseMenuBgSurface = IMG_Load("assets/pause-menu-bg.png");
	pauseMenuBgTexture = SDL_CreateTextureFromSurface(renderer, pauseMenuBgSurface);

	SDL_Surface* continueBtnSurface = IMG_Load("assets/continueBtn.png");
	continueBtnTexture = SDL_CreateTextureFromSurface(renderer, continueBtnSurface);

	scoreFont = TTF_OpenFont("assets/Arial_Bold.ttf", 150);

	SDL_FreeSurface(startMenuSurface);
	SDL_FreeSurface(easyBtnSurface);
	SDL_FreeSurface(mediumBtnSurface);
	SDL_FreeSurface(hardBtnSurface);

	SDL_FreeSurface(menuBgSurface);
	SDL_FreeSurface(menuBtnSurface);
	SDL_FreeSurface(restartBtnSurface);

	SDL_FreeSurface(pauseMenuBgSurface);
	SDL_FreeSurface(continueBtnSurface);
}

// menuId: 0 s'il s'agit du menu princial, 2 s'il s'agit du menu de game over
int checkEvent(int x, int y, int menuId) {
	printf("Menu id: %d\n", menuId);
	// Menu principal
	if (menuId == 0){
		if(collides(easyBtnRect, x, y)) {
			return 0;
		}
		else if(collides(mediumBtnRect, x, y)) {
			return 1;
		}
		else if(collides(hardBtnRect, x, y)) return 2;
	}
	// Menu game over
	else if(menuId == 1) {
		if(collides(menuBtnRect, x, y)) return 3;
		else if(collides(restartBtnRect, x, y)) return 4;
	}
	// Menu pause
	else if(menuId == 2) {
		if(collides(continueBtnRect, x, y)) return 5; 
		else if(collides(menuPauseBtnRect, x, y)) return 3;
	}

	return -1;
}

void renderGameOverMenu(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
	int bgSize = (windowHeight>windowWidth)? windowWidth : windowHeight;
	bgSize *= 0.5;

	SDL_Rect bgRect = {
		(windowWidth - bgSize)/2,
		(windowHeight - bgSize)/2,
		bgSize, bgSize
	};

	float btnWidth = 0.6; // 60% du bgRect
	float btnHeight = btnWidth*261/889; // ratio de l'image buttons: 261/889
	
	menuBtnRect = rect(
		bgRect.x + bgRect.w*(1-btnWidth)/2,
		bgRect.y + bgSize*0.4,
		bgSize*btnWidth, bgSize*btnHeight
	);

	restartBtnRect = rect(
		bgRect.x + bgRect.w*(1-btnWidth)/2,
		bgRect.y + bgSize*0.65,
		bgSize*btnWidth, bgSize*btnHeight
	);

	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 180);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(renderer, NULL);

	SDL_RenderCopy(renderer, gameOverMenuTexture, NULL, &bgRect);
	SDL_RenderCopy(renderer, menuBtnTexture, NULL, &menuBtnRect);
	SDL_RenderCopy(renderer, restartBtnTexture, NULL, &restartBtnRect);
}

void renderPauseMenu(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
	int bgSize = (windowHeight>windowWidth)? windowWidth : windowHeight;
	bgSize *= 0.5;

	SDL_Rect bgRect = {
		(windowWidth - bgSize)/2,
		(windowHeight - bgSize)/2,
		bgSize, bgSize
	};

	float btnWidth = 0.6; // 60% du bgRect
	float btnHeight = btnWidth*261/889; // ratio de l'image buttons: 261/889
	
	continueBtnRect = rect(
		bgRect.x + bgRect.w*(1-btnWidth)/2,
		bgRect.y + bgSize*0.4,
		bgSize*btnWidth, bgSize*btnHeight
	);

	menuPauseBtnRect = rect(
		bgRect.x + bgRect.w*(1-btnWidth)/2,
		bgRect.y + bgSize*0.65,
		bgSize*btnWidth, bgSize*btnHeight
	);

	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 180);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(renderer, NULL);

	SDL_RenderCopy(renderer, pauseMenuBgTexture, NULL, &bgRect);
	SDL_RenderCopy(renderer, continueBtnTexture, NULL, &continueBtnRect);
	SDL_RenderCopy(renderer, menuBtnTexture, NULL, &menuPauseBtnRect);
}

void renderStartMenu(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
	int bgSize = (windowHeight>windowWidth)? windowWidth : windowHeight;
	bgSize *= 0.7;

	SDL_Rect bgRect = {
		(windowWidth - bgSize)/2,
		(windowHeight - bgSize)/2,
		bgSize, bgSize
	};

	float btnWidth = 0.6; // 60% du bgRect
	float btnHeight = btnWidth*261/889; // ratio de l'image buttons: 261/889
	
	easyBtnRect = rect(
		bgRect.x + bgRect.w*(1-btnWidth)/2,
		bgRect.y + bgSize*0.3,
		bgSize*btnWidth, bgSize*btnHeight
	);

	mediumBtnRect = rect(
		bgRect.x + bgRect.w*(1-btnWidth)/2,
		bgRect.y + bgSize*0.5,
		bgSize*btnWidth, bgSize*btnHeight
	);

	hardBtnRect = rect(
		bgRect.x + bgRect.w*(1-btnWidth)/2,
		bgRect.y + bgSize*0.7,
		bgSize*btnWidth, bgSize*btnHeight
	);

	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 180);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(renderer, NULL);

	SDL_RenderCopy(renderer, startMenuTexture, NULL, &bgRect);
	SDL_RenderCopy(renderer, easyBtnTexture, NULL, &easyBtnRect);
	SDL_RenderCopy(renderer, mediumBtnTexture, NULL, &mediumBtnRect);
	SDL_RenderCopy(renderer, hardBtnTexture, NULL, &hardBtnRect);
}

int playerIsDead(Player player, Enemy *enemies, int enemiesCount) {
	int isDead = 0;
	int treshold = 4; // tolerance de la colision

	int playerCenterX = player.x + 20;
	int playerCenterY = player.y + 20*player.h/player.w;
	for(int i=0; i<enemiesCount; i++) {

		int enemyCenterX = enemies[i].x + 20;
		int enemyCenterY = enemies[i].y + 20*enemies[i].h/enemies[i].w;

		if((playerCenterX - enemyCenterX < 20+treshold && playerCenterX - enemyCenterX > -20+treshold &&
			playerCenterY - enemyCenterY < 20-treshold && playerCenterY - enemyCenterY > -20+treshold)) {

			isDead = 1;
			break;
		}
	}

	return isDead;
}

void renderScore(SDL_Renderer* renderer, int score, int windowWidth) {
	SDL_Color scoreColor = {0, 0, 0};

	char *scoreText = malloc(sizeof(char)*15);
	sprintf(scoreText, "Score: %d", score);

	SDL_Surface* scoreSurface = TTF_RenderText_Blended(scoreFont, scoreText, scoreColor);
	
	int margin = windowWidth * 2/100;
	int padding = windowWidth*1/100;
	int scoreWidth = windowWidth * 10/100;
	int scoreHeight = scoreWidth * scoreSurface->clip_rect.h / scoreSurface->clip_rect.w;
	
	SDL_Rect scoreRectContainer = {
		windowWidth - scoreWidth - margin - padding,
		margin - padding,
		scoreWidth+2*padding, scoreHeight+2*padding
	};

	SDL_Rect scoreRect = {
		windowWidth - scoreWidth - margin,
		margin,
		scoreWidth, scoreHeight
	};

	SDL_SetRenderDrawColor(renderer, 255, 255 ,255, 255);
	SDL_RenderFillRect(renderer, &scoreRectContainer);

	SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

	SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

	SDL_FreeSurface(scoreSurface);
	SDL_DestroyTexture(scoreTexture);
}