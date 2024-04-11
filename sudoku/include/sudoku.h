#ifndef SUDOKU_H
#define SUDOKU_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct cell {
    int num;

    int state; /* 0 means invalid, 1 means valid, 2 means pregenerated(unfillable)*/
} Cell;


/*
Cell structure "constructor"
\param num The number in the cell
\return a Cell object containing num
*/
Cell CreateCell(int num);

/*
Randomly shuffle a 9 integers vector

\param vect The vector to shuffle
*/
void shuffle(int *vect, int size);

/*
Generate a randomly generated solvable grid of sudoku with n missing cells

\param grid The grid to fill in place
\param n The number of cell the remove according to the difficulty
*/
void generateGrid(Cell cells[9][9]);

/*
Solve the sudoku, filling the empty cells

\param grid The sudoku grid to solve
*/
void solve(Cell cells[9][9]);

/*
Remove(set to 0) n values at random positions from grid

\param grid The grid to unfill
\param n The number of cell to remove
*/
int* unfill(Cell grid[9][9], int n);


int play(Cell grid[9][9], int x, int y, int value);

int erase(Cell grid[9][9], int x, int y);

int updateGrid(Cell grid[9][9]);

#endif