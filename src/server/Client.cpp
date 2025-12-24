#include "Client.h"

Client::Client(const int in_socket, const sockaddr_in in_address) {
    this->socket = in_socket;
    this->address = in_address;
    this->message = new Message();
}

Client::~Client() {
    delete message;
}
