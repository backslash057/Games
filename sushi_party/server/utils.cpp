#include "utils.hpp"

template <typename Key, typename Value>
list<Key, Value>::list() {};

template <typename Key, typename Value>
list<Key, Value>::Node::Node(Key key, Value* value): key(key), value(value), next(nullptr) {}

template <typename Key, typename Value>
void list<Key, Value>::add(Key key, Value* value) {
    size++;
    Node* newNode = new Node(key, value);

    if (head == NULL) {
        head = newNode;
        return;
    }

    Node* current = head;

    while(current->next != NULL) {
        current = current->next;
    }

    current->next = newNode;
}


template <typename Key, typename Value>
void list<Key, Value>::plot(){
    std::cout << "[";

    if(head != NULL){
        std::cout << '(' << head->value->x<< ", " << head->value->y << ')';

        Node* current = head;
        while(current->next != NULL) {
            std::cout << ", (" << current->value->x<< ", " << current->value->y << ')';
            current = current->next;
        }
    }

    std::cout << "]" << std::endl;
}

/*
template <typename Key, typename Value>
Value list<Key, Value>::get(Key key){
    Node* current = head;

    while(current != NULL) {
        if(current->key == key) return current->value;
        current->next;
    }

    return head->value;
}
*/

template <typename Key, typename Value>
int list<Key, Value>::getSize(){
    return size;
}


template <typename Key, typename Value>
void list<Key, Value>::remove(Key key) {
    if(head == NULL) return;

    if(head->key == key){
        Node *temp = head;
        head = head->next;
        free(temp);
        size--;
    }

    Node* current = head;
    while(current->next != NULL && current->key!=key) {
        current = current->next;
    }

    if(current->next != NULL) {
        Node* deleted = current->next;
        current->next = deleted->next;
        free(deleted);
        size--;
    }
}