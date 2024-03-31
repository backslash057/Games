#include "texture.h"

/*
    Give the snake head texture image filename

    @param vect The snake velocity vector
    @returns The snake head texture filename according to the direction vect
*/
char* headTexturePath(Vector2 vect) {
    if(vect.x == -1 && vect.y == 0) return "head_left.png";
    else if(vect.x == 1 && vect.y == 0) return "head_right.png";
    else if(vect.x == 0 && vect.y == -1) return "head_up.png";
    else return "head_down.png";
}

/*
    Get the snake body part filename
    
    @param vect1 A vector from current part to previous part
    @param vect2 A vector from current part to next part
    @return The name of the corresponding image path
*/
char* bodyTexturePath(Vector2 vect1, Vector2 vect2) {
    if(vect1.x == -vect2.x && vect1.x != 0) return "body_horizontal.png";
    else if(vect1.y == -vect2.y && vect1.y != 0) return "body_vertical.png";

    if((vect1.x==-1 && vect1.y==0 && vect2.x==0 && vect2.y==1) || (vect1.x==0 && vect1.y==1 && vect2.x==-1 && vect2.y==0)) return "body_bottomleft.png";
    else if((vect1.x==1 && vect1.y==0 && vect2.x==0 && vect2.y==1) || (vect1.x==0 && vect1.y==1 && vect2.x==1 && vect2.y==0)) return "body_bottomright.png";
    else if((vect1.x==0 && vect1.y==-1 && vect2.x==-1 && vect2.y==0) || (vect1.x==-1 && vect1.y==0 && vect2.x==0 && vect2.y==-1)) return "body_topleft.png";
    else if((vect1.x==1 && vect1.y==0 && vect2.x==0 && vect2.y==-1) || (vect1.x==0 && vect1.y==-1 && vect2.x==1 && vect2.y==0)) return "body_topright.png";

    return "tail_left.png";
}

/*
    Get the snake body part filename
    
    @param vect A vector from current part(tail) to previous part
    @return The name of the corresponding image path
*/
char* tailTexturePath(Vector2 vect){
    if(vect.x == -1 && vect.y == 0) return "tail_right.png";
    else if(vect.x == 1 && vect.y == 0) return "tail_left.png";
    else if(vect.x == 0 && vect.y == -1) return "tail_down.png";
    else return "tail_up.png";
}



char* fruitTexturePath(int textureId) {
    char* fruits[] = {"apple.png"};

    return fruits[textureId];
}