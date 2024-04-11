#include "sudoku.h"
#include "constants.h"

Cell cell_(int num, int state) {
    Cell cell = {num, state};
    return cell;
}

void shuffle(int *vect, int size) {
    int r, temp;
    for(int i=0; i<size; i++) {
        r = rand()%size;

        temp = vect[i];
        vect[i] = vect[r];
        vect[r] = temp;
    }
}

int isInColumn(Cell grid[9][9], int n, int x){
    for(int y=0; y<9; y++) {
        if(grid[x][y].num == n) return 1;
    }

    return 0;
}

int isInRow(Cell grid[9][9], int n, int y){
    for(int x=0; x<9; x++) {
        if(grid[x][y].num == n) return 1;
    }

    return 0;
}

int isInCell(Cell grid[9][9], int n, int x, int y){
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++){
            if(grid[x-x%3+i][y-y%3+j].num==n) return 1;
        }
    }
    
    return 0;
}

int isValid(Cell grid[9][9], int n, int x, int y) {
    return (
        !isInColumn(grid, n, x) &&
        !isInRow(grid, n, y) &&
        !isInCell(grid, n, x, y)
    );
}

int fillRemaining(Cell grid[9][9], int x, int y)
{
    if(x >= 9 && y >= 8) return 1;
    if(x>=9) {
        x = 0;
        y += 1;
    }

    if(grid[x][y].num != 0) return fillRemaining(grid, x+1, y);

    int vect[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    shuffle(vect, 9);

    for(int i=0; i<9; i++) {
        if(isValid(grid, vect[i], x, y)) {
            grid[x][y] = cell_(vect[i], UNFILLABLE);

            if(fillRemaining(grid, x+1, y)){
                return 1;
            }
            grid[x][y] = cell_(0, UNFILLABLE);
        }
    }
    return 0;
}

void generateGrid(Cell grid[9][9]){
    srand(time(NULL));

    // Fill the 3 diagonal blocks
    for (int k = 0; k < 3; k++){
        int vect[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        shuffle(vect, 9);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                grid[i+3*k][j+3*k] = cell_(vect[i*3+j], UNFILLABLE);
            } 
        }
    }

    // Fill the other cells with 0
    int start, x;
    for(int y=0; y<9; y++){
        start = (y/3 + 1)*3;
        for(int l=0; l<6; l++) {
            x = (start+l)%9;
            grid[x][y] = cell_(0, UNFILLABLE);
        }
    }

    // Fill the other cells starting from x=3 and y=0
    fillRemaining(grid, 3, 0);
    
}

int* unfill(Cell grid[9][9], int n) {
    /*
        This code generate fill and shuffle a 81 int vector. The first n values are
        unfilled from the cells grid. The generic method for this is to generate
        indices until n different values are reached but the disadvantage is the
        variable time complexity and the possible infinite loop.
    */
    int *unfilled = malloc(sizeof(int)*8);
    for(int i=0; i<9; i++) unfilled[i]=0;

    int temp[81];
    for(int i=0; i<81; i++) temp[i] = i;
    shuffle(temp, 81);

    for(int i=0; i<n; i++) {
        unfilled[grid[temp[i]/9][temp[i]%9].num - 1] += 1;
        grid[temp[i]/9][temp[i]%9] = cell_(0, NORMAL);
    }

    return unfilled;
}

int play(Cell grid[9][9], int x, int y, int value) {
    
    if(grid[x][y].state != UNFILLABLE && grid[x][y].num != value) {
        int previous = grid[x][y].num;
        grid[x][y].num = value;

        updateGrid(grid);

        return previous;   
    }

    return -1;
}


int erase(Cell grid[9][9], int x, int y) {
    if(grid[x][y].state == UNFILLABLE) return -1;

    int value = grid[x][y].num;

    grid[x][y].num = 0;
    grid[x][y].state = 1;

    updateGrid(grid);
    return 0;    
}

int updateGrid(Cell grid[9][9]) {
    for(int x=0; x<9; x++){
        for(int y=0; y<9; y++) {
            if(grid[x][y].state == UNFILLABLE) continue;

            int temp = grid[x][y].num;
            grid[x][y].num = 0;
            grid[x][y].state = isValid(grid, temp, x, y);
            grid[x][y].num = temp;
        }
    }
}