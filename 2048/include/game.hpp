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

    void update(std::vector<Game::Cell> cells, int* animSteps);

    void addRandomCell(std::vector<Game::Cell> cells);

    void shiftLeft(std::vector<Game::Cell> cells);
    void shiftUp(std::vector<Game::Cell> cells);
    void shiftRight(std::vector<Game::Cell> cells);
    void shiftDown(std::vector<Game::Cell> cells);

    void stack(std::vector<Game::Cell> cells);
}

#endif