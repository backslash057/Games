#include <iostream>
#include <vector>
#include <algorithm>

struct Cell {
	int x, y;

	int value;

	Cell(int x, int y, int val): x(x), y(y), value(val) {}
	Cell(int x, int y): x(x), y(y), value(2) {}
};

bool lessXComparator(Cell* cell1, Cell* cell2) {
	return cell1->x < cell2->x;
}

bool lessYComparator(Cell* cell1, Cell* cell2) {
	return cell1->y < cell2->y;
}


void shiftUp(std::vector<Cell>& cells, const int gridSize) {
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

void shiftLeft(std::vector<Cell>& cells, const int gridSize) {
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

void shiftRight(std::vector<Cell>& cells, const int gridSize) {
	std::vector<Cell*> rows[gridSize];

	for(auto& cell : cells) rows[cell.y].push_back(&cell);
	for(int i=0; i<gridSize; i++) std::sort(rows[i].begin(), rows[i].end(), lessXComparator);

	for(int i=0; i<gridSize; i++){
		int index=0;
		int j=0;

		while(j < rows[i].size()) {
			rows[i][rows[i].size()-j-1]->x = gridSize - index - 1;

			if(j < rows[i].size()-1 && rows[i][rows[i].size()-j-2]->value == rows[i][rows[i].size()-j-1]->value){
				rows[i][rows[i].size()-j-2]->x = gridSize - index - 1;
				j++;
			}

			j++;
			index++;
		}
	}
}



void shiftDown(std::vector<Cell>& cells, const int gridSize) {
	std::vector<Cell*> columns[gridSize];

	for(auto& cell : cells) columns[cell.x].push_back(&cell);
	for(int i=0; i<gridSize; i++) std::sort(columns[i].begin(), columns[i].end(), lessYComparator);

	for(int i=0; i<gridSize; i++){
		int index=0, j=0;

		while(j < columns[i].size()) {
			columns[i][columns[i].size()-j-1]->y = gridSize - index - 1;

			if(j < columns[i].size()-1 && columns[i][columns[i].size()-j-2]->value == columns[i][columns[i].size()-j-1]->value) {
				columns[i][columns[i].size()-j-2]->y = gridSize - index - 1;
				j++;
			}

			j++;
			index++;
		}
	}
}

void stack(std::vector<Cell>& cells) {
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
}

void addRandomCell(std::vector<Cell>& cells, int gridSize) {
	int index;
	bool contains;

	do {
		index = rand() % (gridSize*gridSize);
		
		contains = false;
		for(auto cell : cells) {
			if(cell.x + gridSize * cell.y == index) contains = true;
		}
	} while(contains);

	Cell cell{index%gridSize, index/gridSize, 2};
	cells.push_back(cell);
}

void display(std::vector<Cell> cells) {
	std::cout << "[";
	if(cells.size() > 0) std::cout << '(' << cells[0].x << ", " << cells[0].y << ", " << cells[0].value << ")";

	for(int i=1; i<cells.size(); i++) {
		std::cout << ", (" << cells[i].x << ", " << cells[i].y << ", " << cells[i].value << ")";
	}
	std::cout << "]" << std::endl;
}

void render(std::vector<Cell> cells, int gridSize) {
	for(int i=0; i<gridSize; i++) {
		for(int j=0; j<gridSize; j++) {
			int value = 0;

			for(auto cell : cells) {
				if(cell.x == j && cell.y == i) value = cell.value;
			}
			std::cout << value << " ";
		}

		std::cout << std::endl;
	}
}


int main() {
	int gridSize = 5;
	std::vector<Cell> cells = {Cell(0, 0, 8), Cell(1, 0, 2), Cell(2, 0, 2)};
	char input;

	while(true) {
		std::cout << std::endl;
		display(cells);
		render(cells, gridSize);

		std::cout << "> ";
		std::cin >> input;

		switch(input) {
		case 'z':
			shiftUp(cells, gridSize);
			break;
		case 'q':
			shiftLeft(cells, gridSize);
			break;
		case 's':
			shiftDown(cells, gridSize);
			break;
		case 'd':
			shiftRight(cells, gridSize);
			break;
		}

		stack(cells);
		addRandomCell(cells, gridSize);
	}

	

	return 0;
}