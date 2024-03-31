#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>
#include <ostream>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <cstring>

typedef struct {
    int conn;
    char *host;
    int port;
} Client;

void client_handler(Client datas);

void update();

#endif