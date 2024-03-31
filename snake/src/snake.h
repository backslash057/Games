#ifndef SNAKE_H
#define SNAKE_H
#define BLOCK_SIZE 50
#define FRUITS_LENGTH 1
#define xSize 17
#define ySize 15

typedef struct block {
    int x, y;
    struct block* next;
} Block;

typedef struct vector {
    int x;
    int y;
} Vector2;

typedef struct snake {
    Vector2 dir;
    Block *head;
} Snake;

typedef struct fruit {
    int x, y;
    int textureId;
} Fruit;

Snake snake_(int x, int y, Vector2 dir, int size);
Fruit fruit_(Snake snake);

void move(Snake *snake, Vector2 dir);
void replace(Fruit *fruit);
void plot(Snake snake);
void grow(Snake *snake, Vector2 dir);
int isAboutToDie(Snake snake, Vector2 dir);
int isAboutToEat(Snake snake, Fruit fruit);

#endif