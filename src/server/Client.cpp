#include "Client.h"

#include "Game.h"
#include "Room.h"
#include "Serializer.h"
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

void Client::addMessageToBuffer(const ServerMessageTypes::Type type, const std::string &payload)
{
    this->message->type = type;
    this->message->length = payload.size();
    this->message->payload = payload;
    Utils::writeDebugLog(this, this->message->payload);
    // Utils::writeMessageAsHex(this->message);
    auto buf = Serializer::serialize(*this->message);
    std::lock_guard<std::mutex> lock(this->sendingBufferMutex);
    this->sendingBuffer.insert(this->sendingBuffer.begin(), buf.begin(), buf.end());
}

std::string Client::addressToString() const
{
    return std::string(inet_ntoa(this->address.sin_addr)) + ":" + std::to_string(ntohs(this->address.sin_port));
}

void Client::tick()
{
    std::lock_guard<std::mutex> lock(pingPongMutex);

    pingIntervalCounterSeconds--;
    if (pingIntervalCounterSeconds <= 0)
    {
        pingIntervalCounterSeconds = PING_INTERVAL_SECONDS;
        pongTimeoutCountersSeconds.push_back(PONG_TIMEOUT);
        // Utils::writeDebugLog(this, "Sending ping");
        this->addMessageToBuffer(ServerMessageTypes::PING, "");
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

        if (room->game && room->game->inProgress == false)
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
    // Utils::writeDebugLog(this, "Pong received");
}
