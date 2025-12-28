#include "Room.h"

#include <algorithm>
#include <iostream>

#include "Game.h"
#include "Server.h"

Room::Room(Server *server, Client *owner)
{
    this->server = server;
    this->owner = owner;
    this->clients.push_back(owner);
    this->id = std::to_string(Room::roomCounter++);
    this->pin = generatePin();
}

Room::~Room()
{
    this->server = nullptr;
    this->owner = nullptr;
    this->clients.clear();
}

std::string Room::generatePin()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);

    const int number = dis(gen);
    return std::to_string(number);
}

void Room::broadcastMessage(const Response::Type type, const std::string& payload) const
{
    for(const auto client : this->clients)
    {
        this->server->sendMessage(client, type, payload);
    }
}

static std::string padLeft(const std::string& str, const size_t totalWidth, const char padChar = ' ')
{
    if (str.length() >= totalWidth)
    {
        return str;
    }
    return std::string(totalWidth - str.length(), padChar) + str;
}

static std::string padRight(const std::string& str, const size_t totalWidth, const char padChar = ' ')
{
    if (str.length() >= totalWidth)
    {
        return str;
    }
    return str + std::string(totalWidth - str.length(), padChar);
}

void Room::broadcastPlayersListLobby() const
{
    std::string message;

    size_t maxNicknameSize = 0;
    for(const auto &client : clients)
    {
        if(client->nickname.size()  > maxNicknameSize)
        {
            maxNicknameSize = client->nickname.size();
        }
    }

    const size_t size = clients.size();
    for(size_t i = 0; i < size; i++)
    {
        const auto client = clients[i];
        message += padRight(client->nickname, maxNicknameSize) + " (" + (client->isConnected ? "connected" : "disconnected") + ")";
        if(i != size - 1)
        {
            message += "|";
        }
    }

    std::cout << "Broadcasting players list for room ID = " << this->id << " list = " << message << std::endl;
    broadcastMessage(Response::ROOM_USERS_LIST, message);
}

void Room::broadcastPlayersListGame() const
{
    std::string message;

    size_t maxNicknameSize = 0;
    for(const auto &client : clients)
    {
        if(client->nickname.size() > maxNicknameSize)
        {
            maxNicknameSize = client->nickname.size();
        }
    }

    const size_t size = clients.size();
    for(size_t i = 0; i < size; i++)
    {
        auto client = clients[i];
        //TODO: implement sending game stats for each user
        message += padRight(client->nickname, maxNicknameSize)  + " (" + (client->isConnected ? "connected" : "disconnected") + ")";
        if(i != size - 1)
        {
            message += "|";
        }
    }

    std::cout << "Broadcasting players list for room ID = " << this->id << " list = " << message << std::endl;
    broadcastMessage(Response::ROOM_USERS_LIST, message);
}

void Room::broadcastPlayersGameStats() const
{
    std::string payload;
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        const auto client = clients[i];
        payload += client->nickname + ":" + (client->isConnected ? "connected" : "disconnected") + ":" + "100" + ":" + std::to_string(client->errors) ;
        if(i != this->clients.size() - 1)
        {
            payload += "|";
        }
    }
    //
    this->broadcastMessage(Response::GAME_STATE, payload);
}

void Room::join(Client *client)
{
    this->clients.push_back(client);
    client->room = this;
    broadcastPlayersListLobby();
}

Client* Room::leave(const Client* client)
{
    const auto iterator = std::find(clients.begin(), clients.end(), client);
    if (iterator == clients.end())
    {
        return nullptr;
    }

    const bool wasOwner = (client == owner);
    clients.erase(iterator);

    if (clients.empty())
    {
        // TODO: close room, remove from server's list
        return nullptr;
    }

    if (wasOwner)
    {
        owner = clients.front();
    }

    if(isGameStarted)
    {
        broadcastPlayersListGame();
    }
    else
    {
        broadcastPlayersListLobby();
    }

    return wasOwner ? owner : nullptr;
}

bool Room::isClientInRoom(const Client *client) const
{
    for (auto &roomClient : this->clients)
    {
        if(roomClient == client)
        {
            return true;
        }
    }
    return false;
}

void Room::startGame()
{
    this->game = Game::create(10, 60);
    isGameStarted = true;
    broadcastMessage(Response::GAME_STARTED, this->game->getGameStartedPayload());
    for (auto client : this->clients)
    {
        client->errors = 0;
    }
}
