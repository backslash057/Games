#include "network.hpp"
#include "player.hpp"

#include <string>
#include <vector>
#include <sstream>

#include "utils.hpp"

int COUNT=0;
list<int, Player> snakes;

void update() {
    int i=0;

    while(true) {
        if(snakes.getSize() == 0) continue;
        snakes.plot();
        sleep(1);
    }
}

void client_handler(Client client) {
    char buffer[1000];
    int x=0, y=0, id=snakes.getSize();


    Player* player = new Player(id, x, y);
    snakes.add(id, player);

    std::cout << client.host << "(" << client.port << ") connected" << std::endl;
    std::cout << client.conn << std::endl;

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        ssize_t bytesRead = read(client.conn, buffer, sizeof(buffer));

        if (bytesRead > 0) {
            // std::cout << "[" << client.host << "(" << client.port <<")] => " << buffer << std::endl;
        
            std::string text(buffer);
            std::istringstream flux(text);
            std::string temp;

            std::getline(flux, temp, ';');
            x = std::stoi(temp);
            std::getline(flux, temp, ';');
            y = std::stoi(temp);

            player->x = x;
            player->y = y;

        } else if (bytesRead == 0) {
            std::cout << client.host << "(" << client.port << ") disconnected" << std::endl;
            break;
        } else {
            perror("Error reading from socket");
            break;
        }
    }

    // Fermez la connexion
    close(client.conn);
}