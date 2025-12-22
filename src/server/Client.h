#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <netinet/in.h>

class Client {
public:
    int socket;
    sockaddr_in address{};
    int closed{};
    std::vector<char> buffer;
    Client(int in_socket, sockaddr_in in_address);
};

#endif