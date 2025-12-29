#ifndef CLIENT_H
#define CLIENT_H

#include "Constants.h"
#include "Message.h"

#include <netinet/in.h>
#include <vector>

class Room;
class Server;

class Client
{
public:
    int socket;
    sockaddr_in address{};
    Message *message;
    std::vector<char> buffer;

    std::string nickname;
    Room *room = nullptr;
    int errors;

    Client(int inSocket, sockaddr_in inAddress);
    ~Client();

    bool isConnected = true;
    bool receivedPong = false;
    int pingIntervalCounterSeconds = PING_INTERVAL_SECONDS;
    std::vector<int> pongTimeoutCountersSeconds;

    std::string addressToString() const;
    void tick(Server *server);
};

#endif
