#ifndef SUDOKU_H
#define SUDOKU_H


typedef struct cell {
    int num;
    int valid;
} Cell;

void solve(int grid[9][9]);


#endif