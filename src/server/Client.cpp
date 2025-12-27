#include "Client.h"

#include <arpa/inet.h>

Client::Client(const int in_socket, const sockaddr_in in_address) {
    this->socket = in_socket;
    this->address = in_address;
    this->message = new Message();
}

Client::~Client() {
    delete message;
}

std::string Client::address_to_string()
{
    return std::string(inet_ntoa(this->address.sin_addr)) + ":" + std::to_string(ntohs(this->address.sin_port));   
}
