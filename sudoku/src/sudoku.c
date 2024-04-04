#include "sudoku.h"


Cell CreateCell(int num) {
    Cell cell = {num, 1};
    return cell;
}


void shuffle(int vect[9]) {
    int r, temp;
    for(int i=0; i<9; i++) {
        r = rand()%9;

        temp = vect[i];
        vect[i] = vect[r];
        vect[r] = temp;
    }
}


void generateGrid(Cell grid[9][9]){
    srand(time(NULL));

    // Fill the 3 diagonal blocks
    for (int k = 0; k < 3; k++){
        int vect[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        shuffle(vect);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                grid[i+3*k][j+3*k] = CreateCell(vect[i*3+j]);
            } 
        }
    }

    // Fill the othe cells
    int start, x;
    for(int y=0; y<9; y++){
        start = (y/3 + 1)*3;
        
        for(int l=0; l<6; l++) {
            x = (start+l)%9;

            int vect[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            shuffle(vect);

            

            grid[x][y] = CreateCell(1);
        }

        printf("\n");
    }
    
}

void solve(Cell grid[9][9]){
    
}