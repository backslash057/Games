#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <vector>
#include <array>
#include <cstdlib>
#include <ctime>

namespace Game {
    struct Cell{
        int value;

        int x, y;
        float animX, animY;

        Cell(): value(0) {}
        Cell(int val, int x, int y): value(val), animX(x), animY(y) {}
    };

    void initRng();

    void update(Cell *cells, int gridSize, int* animSteps);

    void addRandomCell(Cell *cells, int gridSize);

    void shiftLeft(Cell *cells, int gridSize);
    void shiftUp(Cell *cells, int gridSize);
    void shiftRight(Cell *cells, int gridSize);
    void shiftDown(Cell *cells, int gridSize);

    void stack(Cell *cells, int gridSize);
}

#endif