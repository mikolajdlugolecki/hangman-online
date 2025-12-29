#include "Client.h"

#include "Room.h"
#include "Server.h"

#include <arpa/inet.h>

Client::Client(const int inSocket, const sockaddr_in inAddress)
{
    this->socket = inSocket;
    this->address = inAddress;
    this->message = new Message();
}

Client::~Client()
{
    delete message;
}

std::string Client::addressToString() const
{
    return std::string(inet_ntoa(this->address.sin_addr)) + ":" + std::to_string(ntohs(this->address.sin_port));
}

void Client::tick(Server *server)
{
    std::lock_guard<std::mutex> lock(pingPongMutex);

    pingIntervalCounterSeconds--;
    if (pingIntervalCounterSeconds <= 0)
    {
        pingIntervalCounterSeconds = PING_INTERVAL_SECONDS;
        pongTimeoutCountersSeconds.push_back(PONG_TIMEOUT);
        // server->write_debug_log(this, "Sending ping");
        server->sendMessage(this, ServerMessageTypes::PING, "");
    }

    for (auto &p : pongTimeoutCountersSeconds)
    {
        p--;
    }

    if (!pongTimeoutCountersSeconds.empty() && pongTimeoutCountersSeconds[0] <= 0)
    {
        pongTimeoutCountersSeconds.erase(pongTimeoutCountersSeconds.begin());
        receivedPong = false;
    }

    if (isConnected != receivedPong && room != nullptr)
    {
        isConnected = receivedPong;

        if (room->isGameStarted == false)
        {
            room->broadcastPlayersListLobby();
        }
        else
        {
            room->broadcastPlayersListGame();
        }
    }
}

void Client::pongReceived()
{
    std::lock_guard<std::mutex> lock(pingPongMutex);

    receivedPong = true;
    if (isConnected == false)
    {
        pongTimeoutCountersSeconds.clear();
    }
    if (pongTimeoutCountersSeconds.empty() == false)
    {
        pongTimeoutCountersSeconds.erase(pongTimeoutCountersSeconds.begin());
    }
    // writeDebugLog(client, "Pong received");
}
