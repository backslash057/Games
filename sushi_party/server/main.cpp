#include <iostream>
#include <ostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

#include "network.hpp"

#define PORT 12345

int main(int argc, char *argv[]) {
    int sock=0;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation error");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);


    if(bind(sock, (struct sockaddr*)&address, addrlen) == -1) {
        perror("Address and port binding error");
        return -1;
    }


    if(listen(sock, 5) < 0){
        perror("Connexion linstening error");
        return -1;
    }

    std::cout << "Waiting for connexion on port " << PORT << std::endl;

    std::thread updater(update);
    updater.detach();

    while(true){
        int conn = accept(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            
        if(conn < 0) {
            std::cout << "Erreur d'acceptation de connexion";
        }
        else {
            Client client = {conn, inet_ntoa(address.sin_addr), address.sin_port};

            std::thread client_thread(client_handler, client);

            client_thread.detach();
        }
    }
    
    std::cout <<  "Server closing" << std::endl;
    return 0;
}
