#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Message.h"

class Client {
public:
    int socket;
    sockaddr_in address{};
    Message *message;
    std::vector<char> buffer;
    std::string nickname;
    Client(int in_socket, sockaddr_in in_address);
    ~Client();

    std::string address_to_string();
};

#endif