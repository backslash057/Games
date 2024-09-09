#include "game.hpp"


void Game::initRng() {
	srand(time(NULL));
}

void Game::update(std::vector<Cell>& cells, int& animSteps) {
	if(animSteps != 0) {
		for(auto& cell : cells) {
			if(cell.value == 0) continue;

	        if(animSteps == 1){
	            cell.animX = cell.x;
	            cell.animY = cell.y;
	        }
	        else {
	            float stepX = (cell.x - cell.animX) / animSteps;
	            float stepY = (cell.y - cell.animY) / animSteps;

	            cell.animX += stepX;
	            cell.animY += stepY;
	        }
		}

		animSteps--;
	}
}

void Game::addRandomCell(std::vector<Cell>& cells, int gridSize) {
	if(cells.size() == gridSize * gridSize) return;
	
	int index;
	bool contains;

	do {
		index = rand() % (gridSize*gridSize);
		
		contains = false;
		for(auto cell : cells) {
			if(cell.x + gridSize * cell.y == index) contains = true;
		}
	} while(contains);

	Cell cell;
	cell.value = 2;
	cell.x = cell.animX = index%gridSize;
	cell.y = cell.animY = index/gridSize;

	cells.push_back(cell);
}

bool Game::lessXComparator(Cell* cell1, Cell* cell2) {
	return cell1->x < cell2->x;
}

bool Game::lessYComparator(Cell* cell1, Cell* cell2) {
	return cell1->y < cell2->y;
}

void Game::shiftUp(std::vector<Cell>& cells, const int gridSize) {
	std::vector<Cell*> columns[gridSize];

	for(auto& cell : cells) columns[cell.x].push_back(&cell);
	for(int i=0; i<gridSize; i++) std::sort(columns[i].begin(), columns[i].end(), lessYComparator);


	for(int i=0; i<gridSize; i++){
		int index=0, j=0;

		while(j < columns[i].size()) {
			columns[i][j]->y = index;


			if(j < columns[i].size()-1 && columns[i][j+1]->value == columns[i][j]->value) {
				columns[i][j+1]->y = index;
				j++;
				
			}
			j++;
			index++;
		}
	}
}

void Game::shiftLeft(std::vector<Cell>& cells, const int gridSize) {
	std::vector<Cell*> rows[gridSize];

	for(auto& cell : cells) rows[cell.y].push_back(&cell);
	for(int i=0; i<gridSize; i++) std::sort(rows[i].begin(), rows[i].end(), lessXComparator);

	for(int i=0; i<gridSize; i++){
		int index=0;
		int j=0;

		while(j < rows[i].size()) {
			rows[i][j]->x = index;

			if(j < rows[i].size()-1 && rows[i][j+1]->value == rows[i][j]->value){
				rows[i][j+1]->x = index;
				j++;
			}

			j++;
			index++;
		}
	}
}

void Game::shiftRight(std::vector<Cell>& cells, const int gridSize) {
	std::vector<Cell*> rows[gridSize];

	for(auto& cell : cells) rows[cell.y].push_back(&cell);
	for(int i=0; i<gridSize; i++) std::sort(rows[i].begin(), rows[i].end(), lessXComparator);

	for(int i=0; i<gridSize; i++){
		int index=0;
		int j=0;

		while(j < rows[i].size()) {
			rows[i][rows[i].size()-j-1]->x = gridSize - index - 1;

			if(j < rows[i].size()-1 &&
				rows[i][rows[i].size()-j-2]->value == rows[i][rows[i].size()-j-1]->value){

				rows[i][rows[i].size()-j-2]->x = gridSize - index - 1;
				j++;
			}

			j++;
			index++;
		}
	}
}


void Game::shiftDown(std::vector<Cell>& cells, const int gridSize) {
	std::vector<Cell*> columns[gridSize];

	for(auto& cell : cells) columns[cell.x].push_back(&cell);
	for(int i=0; i<gridSize; i++) std::sort(columns[i].begin(), columns[i].end(), lessYComparator);

	for(int i=0; i<gridSize; i++){
		int index=0, j=0;

		while(j < columns[i].size()) {
			columns[i][columns[i].size()-j-1]->y = gridSize - index - 1;

			if(j < columns[i].size()-1 &&
				columns[i][columns[i].size()-j-2]->value == columns[i][columns[i].size()-j-1]->value) {
				columns[i][columns[i].size()-j-2]->y = gridSize - index - 1;
				j++;
			}

			j++;
			index++;
		}
	}
}

void Game::stack(std::vector<Cell>& cells) {
	int i=0, j;

	while(true) {
		if(i >= cells.size()) break;
		j=0;
		while(true) {
			if(j>= cells.size()) break;
			if(i != j && cells[i].x == cells[j].x && cells[i].y == cells[j].y) {
				cells.erase(cells.begin() + j);
				cells[i].value *= 2;
			}
			j++;
		}

		i++;
	}

	std::cout << cells.size() << std::endl;
}