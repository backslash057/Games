#ifndef SNAKE_HPP
#define SNAKE_HPP

class Vector2{
  public:
    int x=0, y=0;
    Vector2(): x(0), y(0) {};
    Vector2(int x, int y): x(x), y(y){}

    bool operator==(Vector2& other) const;
    bool operator!=(const Vector2&  other) const;
};


class Snake {
  public:
    int x = 0;
    int y = 0;
    int radius = 10;
    int speed = 10; // pixels per seconds

  public:
    Snake(int radius);
    Snake(int x, int y);
    Snake(int x, int y, int radius);

    int getX();
    int getY();
    int getRadius();
    void move(int deltaTime, Vector2 direction);
};

#endif