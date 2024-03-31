#include <stdio.h>
#include <stdlib.h>
#include "snake.h"

Snake snake_(int x, int y, Vector2 dir, int size) {
    Snake snake;
    snake.head = malloc(sizeof(Block));
    snake.head->x = x;
    snake.head->y = y;
    snake.dir = dir;

    Block *current = snake.head;
    for(int i=1; i<size; i++){
        Block *new = malloc(sizeof(Block));
        new->next = NULL;
        new->x = x - i*dir.x;
        new->y = y - i*dir.y;

        current->next = new;
        current = current->next;
    }

    return snake;
}

/*
    Generate a snake

    @param snake The snake(to avoid placing fruit on snake blocks positions)
    @param xLimit Positive bound of the x position random value 
    @param yLimit position
*/
Fruit fruit_(Snake snake){
    int x, y, id;

    id = rand()%FRUITS_LENGTH;

    while(1) {
        x = rand() % xSize;
        y = rand() % ySize;

        Block* current = snake.head;
        int collide = 0;
        while(current != NULL) {
            if (current->x==x && current->y == y) collide=1;
            current = current->next;
        }

        if(!collide) break;
    }

    
    Fruit fruit = {x, y, id};
    return fruit;
}

void move(Snake *snake, Vector2 dir){
    if (dir.x != -snake->dir.x && dir.y != -snake->dir.y) snake->dir = dir;

    Block* new = malloc(sizeof(Block));
    new->x = snake->head->x + snake->dir.x;
    new->y = snake->head->y + snake->dir.y;
    new->next = snake->head;

    snake->head = new;

    Block *current = snake->head;

    while(current->next->next != NULL) current = current->next;
    current->next=NULL;
}


int isAboutToDie(Snake snake, Vector2 dir){
    int x = snake.head->x + dir.x;
    int y = snake.head->y + dir.y;

    if(snake.dir.x == -dir.x && snake.dir.y == -dir.y) return 0;

    // if the snake move out of screen
    if(x < 0 || x >= xSize) return 1;
    if(y < 0 || y >= ySize) return 1;

    // if the snake bites himself
    Block* current = snake.head->next;
    while(current != NULL) {
        if(current->x == x && current->y == y && current->next != NULL) return 1;
        current = current->next;
    }

    return 0;
}

int isAboutToEat(Snake snake, Fruit fruit) {
    return snake.head->x == fruit.x && snake.head->y == fruit.y;
}

void grow(Snake *snake, Vector2 dir){
    Block* new = malloc(sizeof(Block));

    new->x = snake->head->x + dir.x;
    new->y = snake->head->y + dir.y;
    new->next = snake->head;

    snake->head = new;
    snake->dir = dir;
}

/*
    Debug function to print the snake inline on the standanrd output

    @param snake The snake
*/
void plot(Snake snake) {
    Block *current = snake.head;

    while(current != NULL) {
        printf("(%d, %d) ", current->x, current->y);

        current = current->next;
    }
    printf("\n");
}


