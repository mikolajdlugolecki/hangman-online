#ifndef CLIENT_H
#define CLIENT_H

#include "Constants.h"
#include "Message.h"
#include "MessageType.h"

#include <deque>
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
    std::vector<char> receivingBuffer;
    std::deque<char> sendingBuffer;

    std::string nickname;
    Room *room = nullptr;

    bool isConnected = true;
    bool receivedPong = false;
    int pingIntervalCounterSeconds = PING_INTERVAL_SECONDS;
    std::vector<int> pongTimeoutCountersSeconds;
    std::mutex pingPongMutex;

    Client(int inSocket, sockaddr_in inAddress);
    ~Client();
    void addMessageToBuffer(ServerMessageTypes::Type type, const std::string &payload);
    std::string addressToString() const;
    void tick();
    void pongReceived();
};

#endif
