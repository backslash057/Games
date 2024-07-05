#include "game.h"


Keyboard keyboard() {
	Keyboard kbd;

	kbd.rowCount = 3;
	kbd.colCounts = malloc(sizeof(int)*kbd.rowCount);
	kbd.colCounts[0] = 10;
	kbd.colCounts[1] = 9;
	kbd.colCounts[2] = 9;

	char letters[] = "QWERTYUIOPASDFGHJKL_ZXCVBNM*"

	kbd.cells = malloc(sizeof(Cell*)*kbd.rowCount);

	for(int i=0; i<kbd.rowCount; i++) {
		kbd.cells[i] = malloc(sizeof(Cell)*kbd.colCounts[i]);

		for(int j=0; j<kbd.colCounts[i]; j++) {
			kbd.cells[i][j].status = UNDISCOVERED;
		}
	}

	return kbd;
}

void renderGrid(SDL_Renderer* renderer, Grid grid) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	for(int i=0; i<grid.lineCount; i++) {
		for(int j=0; j<grid.colCount; j++) {
			SDL_RenderDrawRect(renderer, &grid.rects[i][j]);
		}
	}

}
