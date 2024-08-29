#include "game.hpp"


void Game::initRng() {
	srand(time(NULL));
}

void Game::update(Game::Cell *cells, int gridSize, int* animSteps) {
	if(*animSteps != 0) {
		for(int i=0; i<gridSize*gridSize; i++) {
			if(cells[i].value == 0) continue;

	        if(*animSteps == 1){
	            cells[i].animX = cells[i].x;
	            cells[i].animY = cells[i].y;
	        }
	        else {
	            float stepX = (cells[i].x - cells[i].animX) / *animSteps;
	            float stepY = (cells[i].y - cells[i].animY) / *animSteps;

	            cells[i].animX += stepX;
	            cells[i].animY += stepY;
	        }
		}

		*animSteps = *animSteps - 1;
		if(animSteps == 0) {
            std::cout << "fin d'animation" << std::endl;
            // the animation has eneded, the values should be stacked
            // call the function that stack the element values
            // stack(cells);
        }
	}
}

void Game::addRandomCell(Cell *cells, int gridSize) {
	std::vector<int> options;

	for(int i=0; i<gridSize*gridSize; i++) {
		if(cells[i].value == 0) {
			options.push_back(i);
		}
	}
	
	int index = options.at(rand()%options.size());

	std::cout << "choosen: " << index << std::endl;
	if(options.size() != 0) {
		cells[index].value = 2;
		cells[index].x = cells[index].animX = index%gridSize;
		cells[index].y = cells[index].animY = index/gridSize;
	}

}

void Game::shiftLeft(Cell *cells, int gridSize) {
	std::vector<i> lines[gridSize];


	for(int i=0; i<gridSize * gridSize; i++) {
		if(cells[i].value != 0) {
			lines[cells[i].y].push_back(i);
		}
	}

	// sort the different lines here cells are stored in a list
	// with array representation, cells x indices are already sorted


	int ptr;
	int *previous;
	Cell temp;
	for(int i=0; i<gridSize; i++) {
		ptr = 0;
		previous = NULL;

		for(int j=0; j<gridSize; j++) {
			if(cells[i*gridSize + j].value == 0) {
				temp = cells[i*gridSize + ptr];

				lines[i].erase(0);
				ptr++;
			}
		}

		// std::cout << lines[i].size() << std::endl;
	}

	// recuperer toute les tuiles pout chaque ligne
	// classer les tuiles de chaque ligne en fonction de leur position en x
	// definir un indice et parcourir les tuiles pour 
}

void Game::shiftUp(Cell *cells, int gridSize) {

}

void Game::shiftRight(Cell *cells, int gridSize) {

}

void Game::shiftDown(Cell *cells, int gridSize) {

}

void Game::stack(Cell *cells, int gridSize) {

}