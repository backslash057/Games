#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <vector>
#include <array>
#include <cstdlib>
#include <ctime>
#include <algorithm>

namespace Game {
    struct Cell{
        int value;

        int x, y;
        float animX, animY;

        Cell(): value(0) {}
        Cell(int val, int x, int y): value(val), animX(x), animY(y) {}
    };

    void initRng();

    void update(std::vector<Cell>& cells, int& animSteps);

    void addRandomCell(std::vector<Cell>& cells, int gridSize);

    bool lessXComparator(Cell* cell1, Cell* cell2);
    bool lessYComparator(Cell* cell1, Cell* cell2);

    void shiftLeft(std::vector<Cell>& cells, const int gridSize);
    void shiftUp(std::vector<Cell>& cells, const int gridSize);
    void shiftRight(std::vector<Cell>& cells, const int gridSize);
    void shiftDown(std::vector<Cell>& cells, const int gridSize);

    void stack(std::vector<Cell>& cells);
}

#endif