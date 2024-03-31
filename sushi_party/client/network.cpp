#include "network.hpp"

Connection::Connection(const char* serverHost, int port) {
    sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    connection = socket(AF_INET, SOCK_STREAM, 0);

    if (inet_pton(AF_INET, serverHost, &serverAddr.sin_addr) <= 0) {
        close(connection);
    }

    status = connect(connection, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
}

int Connection::getStatus() const {
    return status;
}

void Connection::send_(std::string data) const {
    send(connection, data.c_str(), data.size(), 0);
}

void Connection::close_() const {
    close(connection);
}