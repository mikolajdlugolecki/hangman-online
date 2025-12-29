#ifndef CLIENT_H
#define CLIENT_H

#include "Constants.h"
#include "Message.h"

#include <netinet/in.h>
#include <vector>
#include <mutex>

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

    bool isConnected = true;
    bool receivedPong = false;
    int pingIntervalCounterSeconds = PING_INTERVAL_SECONDS;
    std::vector<int> pongTimeoutCountersSeconds;
    std::mutex pingPongMutex;

    Client(int inSocket, sockaddr_in inAddress);
    ~Client();

    std::string addressToString() const;
    void tick(Server *server);
    void pongReceived();
};

#endif
