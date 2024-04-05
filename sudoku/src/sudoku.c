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

int isInColumn(Cell grid[9][9], int n, int x){
    for(int y=0; y<9; y++) {
        if(grid[y][x].num == n) return 1;
    }

    return 0;
}

int isInRow(Cell grid[9][9], int n, int y){
    for(int x=0; x<9; x++) {
        if(grid[y][x].num == n) return 1;
    }

    return 0;
}

int isInCell(Cell grid[9][9], int n, int x, int y){
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++){
            if(grid[y-y%3+j][x-x%3+i].num==n) return 1;
            //printf("(%d %d) -> (%d %d)\n", x, y, x-x%3+i, y-y%3+j);
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

    if(grid[y][x].num != 0) return fillRemaining(grid, x+1, y);

    int vect[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    shuffle(vect);

    for(int i=0; i<9; i++) {
        if(isValid(grid, vect[i], x, y)) {
            grid[y][x].num = vect[i];

            if(fillRemaining(grid, x+1, y)){
                return 1;
            }

            grid[y][x].num = 0;
        }
    }
    return 0;
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

    // Fill the other cells with 0
    int start, x;
    for(int y=0; y<9; y++){
        start = (y/3 + 1)*3;
        for(int l=0; l<6; l++) {
            x = (start+l)%9;
            grid[x][y] = CreateCell(0);
        }
    }

    // Fill the other cells starting from x=3 and y=0
    fillRemaining(grid, 3, 0);
    
}

void unfill(Cell grid[9][9], int n) {
    /*
        This code is only for avoiding the unknown complexity and 
        uncontrolable time to generate the random indices to remove from
        the grid
    */
    int temp[81];
    for(int i=0; i<81; i++) {
        temp[i] = rand()%1000;
    }

    int idMax;
    for(int i=0; i<n; i++) {
        idMax = 0;
        for(int j=0; j<81; j++) {
            if(temp[idMax] < temp[j]) {
                idMax = j;
            }
        }

        temp[idMax] = 0;
        grid[idMax/9][idMax%9].num = 0;
    }
}

/*
    - Fill the diagonal blocks
    - Fill the orther cells block per block

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

    // Fill the other cells
    int start, x;
    for(int y=0; y<9; y++){
        start = (y/3 + 1)*3;
        for(int l=0; l<6; l++) {
            x = (start+l)%9;
            grid[x][y] = CreateCell(0);
        }
    }

    // Fill the othe cells
    int y;
    for(int a=0; a<3; a++){
        for(int b=1; b<3; b++) {
            int k = (a+b)%3;

            for(int i=0; i<3; i++) {
                for(int j=0; j<3; j++) {
                    x = k*3 + i;
                    y = a*3 + j;
                    
                    int vect[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
                    shuffle(vect);

                    int index=0;
                    while(index < 9) {
                        if(
                            !isInColumn(grid, vect[index], x) &&
                            !isInRow(grid, vect[index], y) &&
                            !isInCell(grid, vect[index], x, y)
                        ) break;
                        else index++;
                    }

                    if(index < 9){
                        grid[x][y] = CreateCell(vect[index]);
                    }
                    else {
                        grid[x][y] = CreateCell(0);
                    }

                }
            }
            printf("\n");
        }
    }  
}
*/


/*
    - Fill the diagonal blocks
    - Fill the orther cells line per line

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

    // Fill the other cells
    int start, x;
    for(int y=0; y<9; y++){
        start = (y/3 + 1)*3;
        for(int l=0; l<6; l++) {
            x = (start+l)%9;
            grid[x][y] = CreateCell(0);
        }
    }

    // Fill the othe cells
    for(int y=0; y<9; y++){
        start = (y/3 + 1)*3;
        
        for(int l=0; l<6; l++) {
            x = (start+l)%9;

            int vect[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            shuffle(vect);

            int index=0;
            while(index < 9) {
                if(
                    !isInColumn(grid, vect[index], x) &&
                    !isInRow(grid, vect[index], y) &&
                    !isInCell(grid, vect[index], x, y)
                ) break;
                else index++;
            }

            grid[x][y] = (index < 9)? CreateCell(vect[index]), CreateCell(0);
        }
    }
}

*/