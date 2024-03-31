#include "snake.hpp"

Snake::Snake(int r): radius(r) {}
Snake::Snake(int x, int y): x(x), y(y) {}
Snake::Snake(int x, int y, int r): x(x), y(y), radius(r){}

void Snake::move(int deltaTime, Vector2 direction){
  x += speed * direction.x;
  y += speed * direction.y;
}

int Snake::getX(){
  return x;
} 

int Snake::getY(){
  return y;
}

int Snake::getRadius() {
  return radius;
}

bool Vector2::operator==(Vector2& other) const {
  return x==other.x && y==other.y;
}

bool Vector2::operator!=(const Vector2& other) const {
  return x!=other.x || y!=other.y;
}