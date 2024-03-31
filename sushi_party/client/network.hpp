#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <ostream>
#include <istream>
#include <iostream>
#include <string>

#include <unistd.h>
#include <arpa/inet.h>


class Connection{
    private:
        int connection;
        int status;

    public:
        Connection(const char* host, int port);
        int getStatus() const;
        void send_(std::string message) const;
        void close_() const;
};


#endif