#ifndef SNAKE_HPP
#define SNAKE_HPP

class Player {
  public:
    int id;
    int x = 0;
    int y = 0;

  public:
    Player(int id, int x, int y);

    void move(int x, int y);
};

#endif