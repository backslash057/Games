#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include "player.hpp"

template <typename Key, typename Value>
class list {
    private:
        struct Node {
            Key key;
            Value* value;
            Node* next;
            Node(Key key, Value* value);
        };

        int size=0;
        Node* head = NULL;

    public:
        list();
        int getSize();
        void add(Key key, Value* value);
        void plot();
        // Value get(Key key);
        void remove(Key key);
};


template class list<int, Player>;

#endif