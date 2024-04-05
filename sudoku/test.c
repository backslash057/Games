#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "sudoku.h"

int test(int argc, char *argv[]) {
	Cell grid[9][9];
	int sumX;

	generateGrid(grid);

	for(int i=0; i<9; i++) {
		sumX = 0;
		for(int j=0; j<9; j++) {
			printf("%d ", grid[i][j].num);
			sumX += grid[i][j].num;
		}
		printf("\t%d\n", sumX);
	}

	return 0;
}