#ifndef TEXTURE_H
#define TEXTURE_H
#include "snake.h"



char* headTexturePath(Vector2);

char* bodyTexturePath(Vector2, Vector2);
char* tailTexturePath(Vector2);

char* fruiTexturePath(int textureId);

#endif